#include "../../includes/server.hpp"

// Function to manage the PART command (allow a user to leave a channel)
void Server::PART(std::string message, int fd)
{
    std::vector<std::string> channels;
    std::string reason;
    bool flag;
    
    // Extract parameters and validate
    if (!splitPartParams(message, channels, reason, fd))
    {
        notifyClient3(461, getClientByFd(fd)->getNickname(), getClientByFd(fd)->getFduser(), " :Not enough parameters\r\n");
        return;
    }

    for (size_t i = 0; i < channels.size(); i++)
    {
        flag = false;
        for (size_t j = 0; j < this->channel.size(); j++)
        {
            if (this->channel[j].getChannelName() == channels[i])
            {
                flag = true;
                if (!channel[j].getUserByFd(fd) && !channel[j].getOperatorByFd(fd))
                {
                    notifyClient2(442, getClientByFd(fd)->getNickname(), "#" + channels[i], getClientByFd(fd)->getFduser(), " :You're not on that channel\r\n");
                    continue;
                }
                if (channel[j].getFindUserByName(getClientByFd(fd)->getNickname())->isChannelFounder())
                    channel[j].getFindUserByName(getClientByFd(fd)->getNickname())->setChannelFounder(false);
                std::stringstream ss;
                ss << ":" << getClientByFd(fd)->getNickname() << "!~" << getClientByFd(fd)->getUser() << "@localhost PART #" << channels[i];
                if (!reason.empty())
                    ss << " :" << reason << "\r\n";
                else
                    ss << "\r\n";
                channel[j].broadcastMessage(ss.str());
                if (channel[j].getOperatorByFd(channel[j].getFindUserByName(getClientByFd(fd)->getNickname())->getFduser()))
                    channel[j].removeOperatorByFd(channel[j].getFindUserByName(getClientByFd(fd)->getNickname())->getFduser());
                else
                    channel[j].removeUserByFd(channel[j].getFindUserByName(getClientByFd(fd)->getNickname())->getFduser());
                if (channel[j].getUserCount() == 0)
                    channel.erase(channel.begin() + j);
                else
                {
                    if (!channel[j].hasOperators())
                    {
                        channel[j].promoteFirstUserToOperator();
                    }
                }
            }
        }
        if (!flag)
            notifyClient2(403, getClientByFd(fd)->getNickname(), "#" + channels[i], getClientByFd(fd)->getFduser(), " :No such channel\r\n");
    }
}

// Function to separate the PART command into individual parameters and extract the leaving reason
// int Server::splitPartParams(std::string message, std::vector<std::string> &channels, std::string &reason, int fd)
// {
//     std::string str;
//     std::string tmp;

//     reason = extractPartReason(message, channels);
//     if (channels.size() < 2)
//     {
//         channels.clear();
//         return 0;
//     }
//     channels.erase(channels.begin());
//     str = channels[0];
//     channels.clear();
//     for (size_t i = 0; i < str.size(); i++)
//     {
//         if (str[i] == ',')
//         {
//             channels.push_back(tmp);
//             tmp.clear();
//         }
//         else
//             tmp += str[i];
//     }
//     channels.push_back(tmp);
//     for (size_t i = 0; i < channels.size(); i++)
//     {
//         if (channels[i].empty())
//             channels.erase(channels.begin() + i--);
//     }
//     if (reason[0] == ':')
//         reason.erase(reason.begin());
//     else
//     {
//         for (size_t i = 0; i < reason.size(); i++)
//         {
//             if (reason[i] == ' ')
//             {
//                 reason = reason.substr(0, i);
//                 break;
//             }
//         }
//     }
//     for (size_t i = 0; i < channels.size(); i++)
//     {
//         if (*(channels[i].begin()) == '#')
//             channels[i].erase(channels[i].begin());
//         else
//         {
//             notifyClient2(403, getClientByFd(fd)->getNickname(), channels[i], getClientByFd(fd)->getFduser(), " :No such channel\r\n");
//             channels.erase(channels.begin() + i--);
//         }
//     }
//     return 1;
// }

int Server::splitPartParams(const std::string& message, std::vector<std::string>& channels, std::string& reason, int fd) {
    size_t colonPos = message.find(" :");
    std::string params = (colonPos == std::string::npos) ? message : message.substr(0, colonPos);
    reason = (colonPos == std::string::npos) ? "" : message.substr(colonPos + 2);

    // Split by spaces
    std::stringstream ss(params);
    std::string token;
    while (ss >> token) {
        channels.push_back(token);
    }

    if (channels.size() < 2) {
        return 0;  // Not enough parameters
    }

    // Remove the command name
    channels.erase(channels.begin());

    // Split channels by commas
    std::vector<std::string> parsedChannels;
    for (std::vector<std::string>::iterator it = channels.begin(); it != channels.end(); ++it) {
        size_t start = 0, end;
        while ((end = it->find(',', start)) != std::string::npos) {
            parsedChannels.push_back(it->substr(start, end - start));
            start = end + 1;
        }
        parsedChannels.push_back(it->substr(start));
    }

    // Validate channel names
    channels.clear();
    for (std::vector<std::string>::iterator it = parsedChannels.begin(); it != parsedChannels.end(); ++it) {
        if (!it->empty() && (*it)[0] == '#') {
            channels.push_back(it->substr(1));
        } else {
            notifyClient2(403, getClientByFd(fd)->getNickname(), *it, getClientByFd(fd)->getFduser(), " :No such channel\r\n");
        }
    }

    return 1;
}


// Function to split the parameters in two (channel and reason)
// and store them in the param vector. Then extract and return the reason.
// std::string Server::extractPartReason(std::string &message, std::vector<std::string> &channels)
// {
//     int count;
//     std::stringstream ss(message);
//     std::string str; 
//     std::string reason;

//     count = 2;
//     while (ss >> str && count--)
//         channels.push_back(str);
//     if (channels.size() != 2)
//         return std::string("");
//     partReason(message, channels[1], reason);

//     return reason;
// }

// Function to find the first occurrence of tofind in message
// and store everything after this occurrence in reason.
// void Server::partReason(std::string message, std::string tofind, std::string &reason)
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
//         reason = message.substr(i);
//     i = 0;
//     for (; i < reason.size() && reason[i] == ' '; i++);
//     reason = reason.substr(i);
// }

void Server::partReason(const std::string& message, const std::string& tofind, std::string& reason) {
    // Find the starting position of "tofind" in "message"
    size_t pos = message.find(tofind);
    if (pos != std::string::npos) {
        // Find the position after "tofind"
        pos += tofind.size();
        
        // Extract the substring starting from the position after "tofind"
        reason = message.substr(pos);
        
        // Trim leading spaces
        size_t start = reason.find_first_not_of(' ');
        if (start != std::string::npos) {
            reason = reason.substr(start);
        } else {
            reason.clear(); // If no non-space characters, clear the reason
        }
    } else {
        reason.clear(); // If "tofind" is not found, clear the reason
    }
}
