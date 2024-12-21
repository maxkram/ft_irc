#include "../../includes/server.hpp"

// Method to handle the KICK command
void Server::KICK(std::string message, int fd)
{
    std::vector<std::string> param;
    std::string comment;
    std::string user;
    Channel *chan;

    comment = splitKickParams(message, param, user, fd);
    if (user.empty())
    {
        notifyClient3(461, getClientByFd(fd)->getNickname(), getClientByFd(fd)->getFduser(), " :Not enough parameters\r\n");
        return;
    }
    for (size_t i = 0; i < param.size(); i++)
    {
        if (getChannel(param[i]))
        {
            chan = getChannel(param[i]);
            if (!chan->getUserByFd(fd) && !chan->getOperatorByFd(fd))
            {
                notifyClient2(442, getClientByFd(fd)->getNickname(), "#" + param[i], getClientByFd(fd)->getFduser(), " :You're not on that channel\r\n");
                continue;
            }
            if (chan->getOperatorByFd(fd))
            {
                if (chan->getFindUserByName(user))
                {
                    std::stringstream ss;
                    ss << ":" << getClientByFd(fd)->getNickname() << "!~" << getClientByFd(fd)->getUser() << "@localhost KICK #" << param[i] << " " << user;
                    if (!comment.empty())
                        ss << " :" << comment << "\r\n";
                    else
                        ss << "\r\n";
                    chan->broadcastMessage(ss.str());
                    if (chan->getOperatorByFd(chan->getFindUserByName(user)->getFduser()))
                        chan->removeOperatorByFd(chan->getFindUserByName(user)->getFduser());
                    else
                        chan->removeUserByFd(chan->getFindUserByName(user)->getFduser());
                    if (chan->getUserCount() == 0)
                        channel.erase(channel.begin() + i);
                }
                else
                {
                    notifyClient2(441, getClientByFd(fd)->getNickname(), "#" + param[i], getClientByFd(fd)->getFduser(), " :They aren't on that channel\r\n");
                    continue;
                }
            }
            else
            {
                notifyClient2(482, getClientByFd(fd)->getNickname(), "#" + param[i], getClientByFd(fd)->getFduser(), " :You're not channel operator\r\n");
                continue;
            }
        }
        else
        {
            notifyClient2(403, getClientByFd(fd)->getNickname(), "#" + param[i], getClientByFd(fd)->getFduser(), " :No such channel\r\n");
        }
    }
}

// Method to split the KICK command and extract its parameters
// (channel names and comment)
// std::string Server::splitKickParams(std::string message, std::vector<std::string> &param, std::string &user, int fd)
// {
//     std::string comment;
//     std::string str;
//     std::string tmp;

//     comment = kickReason(message, param);
//     if (param.size() < 3)
//         return std::string("");
//     param.erase(param.begin());
//     str = param[0];
//     user = param[1];
//     param.clear();
//     for (size_t i = 0; i < str.size(); i++)
//     {
//         if (str[i] == ',')
//         {
//             param.push_back(tmp);
//             tmp.clear();
//         }
//         else
//             tmp += str[i];
//     }
//     param.push_back(tmp);
//     for (size_t i = 0; i < param.size(); i++)
//     {
//         if (param[i].empty())
//             param.erase(param.begin() + i--);
//     }
//     if (comment[0] == ':')
//         comment.erase(comment.begin());
//     else
//     {
//         for (size_t i = 0; i < comment.size(); i++)
//         {
//             if (comment[i] == ' ')
//             {
//                 comment = comment.substr(0, i);
//                 break;
//             }
//         }
//     }
//     for (size_t i = 0; i < param.size(); i++)
//     {
//         if (*(param[i].begin()) == '#')
//             param[i].erase(param[i].begin());
//         else
//         {
//             notifyClient2(403, getClientByFd(fd)->getNickname(), param[i], getClientByFd(fd)->getFduser(), " :No such channel\r\n");
//             param.erase(param.begin() + i--);
//         }
//     }
//     return comment;
// }

std::string Server::splitKickParams(std::string message, std::vector<std::string> &channels, std::string &user, int fd) {
    std::string comment;
    std::vector<std::string> params;

    // Extract the reason and parameters
    comment = kickReason(message, params);

    // Ensure the command has enough parameters
    if (params.size() < 3) {
        return "";
    }

    // Parse channels and the target user
    std::string channelStr = params[1];
    user = params[2];
    channels = splitByDelimiter(channelStr, ',');

    // Validate each channel name
    for (std::vector<std::string>::iterator it = channels.begin(); it != channels.end(); ) {
        if ((*it)[0] != '#') {
            notifyClient2(403, getClientByFd(fd)->getNickname(), *it, getClientByFd(fd)->getFduser(), " :No such channel\r\n");
            it = channels.erase(it);
        } else {
            ++it;
        }
    }

    return comment;
}

std::vector<std::string> Server::splitByDelimiter(const std::string &input, char delimiter) {
    std::vector<std::string> result;
    std::string token;
    std::istringstream ss(input);

    while (std::getline(ss, token, delimiter)) {
        result.push_back(token);
    }
    return result;
}

// // Method to split parameters into two (channel and comment) and store them in param
// // then extract and return the comment
std::string Server::kickReason(std::string &message, std::vector<std::string> &params) {
    std::istringstream ss(message);
    std::string word;
    std::string comment;

    // Extract the first three parameters
    for (int i = 0; i < 3 && ss >> word; ++i) {
        params.push_back(word);
    }

    // Extract the rest as the comment
    std::getline(ss, comment);
    if (!comment.empty() && comment[0] == ':') {
        comment.erase(0, 1); // Remove leading colon
    }

    return comment;
}


// // Method to split parameters into two (channel and comment) and store them in param
// // then extract and return the comment
// std::string Server::kickReason(std::string &message, std::vector<std::string> &param)
// {
//     int count;
//     std::stringstream ss(message);
//     std::string str;
//     std::string comment;

//     count = 3;
//     while (ss >> str && count--)
//         param.push_back(str);
//     if (param.size() != 3)
//         return std::string("");
//     kickReason2(message, param[2], comment);
//     return comment;
// }

// // Method to find the first occurrence of tofind in message
// // and store everything after this occurrence in comment
// void Server::kickReason2(std::string message, std::string tofind, std::string &comment)
// {
//     size_t i;
//     std::string str;

//     i = 0;
//     for (; i < message.size(); i++)
//     {
//         if (message[i] != ' ')
//         {
//             for (; i < message.size() && message[i] != ' '; i++)
//                 str += message[i];
//             if (str == tofind)
//                 break;
//             else
//                 str.clear();
//         }
//     }
//     if (i < message.size())
//         str = message.substr(i);
//     i = 0;
//     for (; i < comment.size() && comment[i] == ' '; i++);
//     comment = comment.substr(i);
// }
