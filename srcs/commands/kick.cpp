#include "../../includes/server.hpp"

void Server::KICK(std::string message, int fd)
{
    std::vector<std::string> param;
    std::string user; // Ensure `user` is declared before use
    std::string comment = splitKickParams(message, param, user, fd);
    Channel *chan = NULL;

    if (user.empty())
    {
        sendError(461, getClientByFd(fd)->getNickname(), getClientByFd(fd)->getFduser(), " :Not enough parameters\r\n");
        return;
    }

    for (size_t i = 0; i < param.size(); ++i)
    {
        chan = getChannel(param[i]);
        if (!chan)
        {
            sendErrorToClient(403, getClientByFd(fd)->getNickname(), "#" + param[i], getClientByFd(fd)->getFduser(), " :No such channel\r\n");
            continue;
        }

        if (!chan->getUserByFd(fd) && !chan->getOperatorByFd(fd))
        {
            sendErrorToClient(442, getClientByFd(fd)->getNickname(), "#" + param[i], getClientByFd(fd)->getFduser(), " :You're not on that channel\r\n");
            continue;
        }

        if (!chan->getOperatorByFd(fd))
        {
            sendErrorToClient(482, getClientByFd(fd)->getNickname(), "#" + param[i], getClientByFd(fd)->getFduser(), " :You're not channel operator\r\n");
            continue;
        }

        if (!chan->getFindUserByName(user))
        {
            sendErrorToClient(441, getClientByFd(fd)->getNickname(), "#" + param[i], getClientByFd(fd)->getFduser(), " :They aren't on that channel\r\n");
            continue;
        }

        // Construct and broadcast the KICK message
        std::stringstream ss;
        ss << ":" << getClientByFd(fd)->getNickname()
           << "!~" << getClientByFd(fd)->getUser()
           << "@localhost KICK #" << param[i] << " " << user;

        if (!comment.empty())
            ss << " :" << comment << "\r\n";
        else
            ss << "\r\n";

        chan->broadcastMessage(ss.str());

        // Remove the user from the channel
        if (chan->getOperatorByFd(chan->getFindUserByName(user)->getFduser()))
            chan->removeOperatorByFd(chan->getFindUserByName(user)->getFduser());
        else
            chan->removeUserByFd(chan->getFindUserByName(user)->getFduser());

        // Remove the channel if empty
        if (chan->getUserCount() == 0)
            channel.erase(channel.begin() + i);
    }
}

std::string Server::splitKickParams(std::string message, std::vector<std::string> &param, std::string &user, int fd)
{
    std::string comment = extractKickReason(message, param);
    if (param.size() < 3)
        return std::string("");

    // Extract and process the channel list and user
    param.erase(param.begin()); // Remove the command
    std::string channelList = param[0];
    user = param[1];
    param.clear();

    // Split the channel list by ','
    std::string temp;
    for (size_t i = 0; i < channelList.size(); ++i)
    {
        if (channelList[i] == ',')
        {
            param.push_back(temp);
            temp.clear();
        }
        else
        {
            temp += channelList[i];
        }
    }
    if (!temp.empty())
        param.push_back(temp);

    // Remove empty channel names
    for (size_t i = 0; i < param.size(); ++i)
    {
        if (param[i].empty())
            param.erase(param.begin() + i--);
    }

    // Process the comment
    if (!comment.empty() && comment[0] == ':')
    {
        comment.erase(comment.begin());
    }
    else
    {
        size_t spacePos = comment.find(' ');
        if (spacePos != std::string::npos)
            comment = comment.substr(0, spacePos);
    }

    // Validate channels and remove invalid ones
    for (size_t i = 0; i < param.size(); ++i)
    {
        if (!param[i].empty() && param[i][0] == '#')
        {
            param[i].erase(param[i].begin());
        }
        else
        {
            sendErrorToClient(403, getClientByFd(fd)->getNickname(), param[i], getClientByFd(fd)->getFduser(), " :No such channel\r\n");
            param.erase(param.begin() + i--);
        }
    }

    return comment;
}

std::string Server::extractKickReason(std::string &message, std::vector<std::string> &param)
{
    std::istringstream ss(message);
    std::string str;
    std::string comment;

    // Extract the first three parameters from the message
    for (int count = 3; count > 0 && ss >> str; --count)
        param.push_back(str);

    // Ensure exactly three parameters were extracted
    if (param.size() != 3)
        return std::string("");

    // Append the kick reason from the message
    appendKickReason(message, param[2], comment);
    return comment;
}

void Server::appendKickReason(std::string message, std::string tofind, std::string &comment)
{
    size_t pos = 0;
    std::string currentWord;

    // Find the target word in the message
    while (pos < message.size())
    {
        if (message[pos] != ' ')
        {
            currentWord.clear();
            while (pos < message.size() && message[pos] != ' ')
                currentWord += message[pos++];

            if (currentWord == tofind)
                break;
        }
        else
        {
            ++pos;
        }
    }

    // Extract the comment if the target word is found
    if (pos < message.size())
        comment = message.substr(pos);

    // Remove leading spaces from the comment
    size_t nonSpacePos = comment.find_first_not_of(' ');
    if (nonSpacePos != std::string::npos)
        comment = comment.substr(nonSpacePos);
    else
        comment.clear(); // If the comment is all spaces, clear it
}
