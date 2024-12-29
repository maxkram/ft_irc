#include "../../includes/server.hpp"

void Server::PART(std::string message, int fd)
{
    std::vector<std::string> param;
    std::string reason;

    // Validate parameters
    if (!splitPartParams(message, param, reason, fd))
    {
        sendError(461, getClientByFd(fd)->getNickname(), getClientByFd(fd)->getFduser(), " :Not enough parameters\r\n");
        return;
    }

    // Iterate over each channel in the parameters
    for (size_t i = 0; i < param.size(); ++i)
    {
        bool channelFound = false;

        for (size_t j = 0; j < this->channel.size(); ++j)
        {
            if (this->channel[j].getChannelName() == param[i])
            {
                channelFound = true;

                // Check if the user is in the channel
                if (!channel[j].getUserByFd(fd) && !channel[j].getOperatorByFd(fd))
                {
                    sendErrorToClient(442, getClientByFd(fd)->getNickname(), "#" + param[i], getClientByFd(fd)->getFduser(), " :You're not on that channel\r\n");
                    continue;
                }

                // Handle channel founder status
                User *user = channel[j].getFindUserByName(getClientByFd(fd)->getNickname());
                if (user && user->isChannelFounder())
                {
                    user->setChannelFounder(false);
                }

                // Construct and broadcast the PART message
                std::stringstream ss;
                ss << ":" << getClientByFd(fd)->getNickname()
                   << "!~" << getClientByFd(fd)->getUser()
                   << "@localhost PART #" << param[i];
                if (!reason.empty())
                    ss << " :" << reason << "\r\n";
                else
                    ss << "\r\n";
                channel[j].broadcastMessage(ss.str());

                // Remove user or operator from the channel
                int userFd = user ? user->getFduser() : -1;
                if (channel[j].getOperatorByFd(userFd))
                {
                    channel[j].removeOperatorByFd(userFd);
                }
                else
                {
                    channel[j].removeUserByFd(userFd);
                }

                // Handle empty channels and promote operators if needed
                if (channel[j].getUserCount() == 0)
                {
                    channel.erase(channel.begin() + j);
                }
                else if (!channel[j].hasOperators())
                {
                    channel[j].promoteFirstUser();
                }
            }
        }

        // If the channel wasn't found
        if (!channelFound)
        {
            sendErrorToClient(403, getClientByFd(fd)->getNickname(), "#" + param[i], getClientByFd(fd)->getFduser(), " :No such channel\r\n");
        }
    }
}


int Server::splitPartParams(std::string message, std::vector<std::string> &param, std::string &reason, int fd)
{
    // Extract the reason and validate parameter size
    reason = extractPartReason(message, param);
    if (param.size() < 2)
    {
        param.clear();
        return 0;
    }

    // Separate parameters from the first extracted string
    std::string str = param[1];
    param.clear();
    std::string tmp;

    for (size_t i = 0; i < str.size(); ++i)
    {
        if (str[i] == ',')
        {
            param.push_back(tmp);
            tmp.clear();
        }
        else
        {
            tmp += str[i];
        }
    }
    if (!tmp.empty())
        param.push_back(tmp);

    // Remove empty parameters
    for (size_t i = 0; i < param.size(); ++i)
    {
        if (param[i].empty())
            param.erase(param.begin() + i--);
    }

    // Clean the reason string
    if (!reason.empty() && reason[0] == ':')
    {
        reason.erase(reason.begin());
    }
    else
    {
        for (size_t i = 0; i < reason.size(); ++i)
        {
            if (reason[i] == ' ')
            {
                reason = reason.substr(0, i);
                break;
            }
        }
    }

    // Validate channels and remove invalid ones
    for (size_t i = 0; i < param.size(); ++i)
    {
        if (param[i].empty() || param[i][0] != '#')
        {
            sendErrorToClient(403, getClientByFd(fd)->getNickname(), param[i], getClientByFd(fd)->getFduser(), " :No such channel\r\n");
            param.erase(param.begin() + i--);
        }
        else
        {
            param[i].erase(param[i].begin()); // Remove '#' prefix
        }
    }

    return 1;
}

std::string Server::extractPartReason(std::string &message, std::vector<std::string> &param)
{
    int count = 2;
    std::istringstream ss(message);
    std::string str;
    std::string reason;

    // Extract the first two words from the message
    while (count > 0 && ss >> str)
    {
        param.push_back(str);
        --count;
    }

    // Validate that exactly two parameters were extracted
    if (param.size() != 2)
        return std::string("");

    // Extract the reason using partReason
    partReason(message, param[1], reason);

    return reason;
}

void Server::partReason(std::string message, std::string tofind, std::string &reason)
{
    size_t i = 0;
    std::string currentWord;

    // Find the target word in the message
    while (i < message.size())
    {
        if (message[i] != ' ')
        {
            // Collect characters for the current word
            currentWord.clear();
            while (i < message.size() && message[i] != ' ')
                currentWord += message[i++];

            // Check if the collected word matches the target
            if (currentWord == tofind)
                break;
        }
        else
        {
            ++i;
        }
    }

    // Extract the reason if the target word is found
    if (i < message.size())
    {
        reason = message.substr(i);
    }

    // Remove leading spaces from the reason
    size_t j = 0;
    while (j < reason.size() && reason[j] == ' ')
        ++j;

    reason = reason.substr(j);
}
