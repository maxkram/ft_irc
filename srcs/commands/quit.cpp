#include "../../includes/server.hpp"

// Handles the QUIT command for disconnecting a user
void Server::QUIT(std::string message, int fd)
{
    std::string reply;

    std::string reason = quitReason(message);
    for (size_t i = 0; i < channel.size(); i++)
    {
        if (channel[i].getUserByFd(fd))
        {
            channel[i].removeUserByFd(fd);
            if (channel[i].getUserCount() == 0)
                channel.erase(channel.begin() + i);
            else
            {
                reply = ":" + getClientByFd(fd)->getNickname() + "!~" + getClientByFd(fd)->getUser() + "@localhost QUIT " + reason + "\r\n";
                channel[i].broadcastMessage(reply);
            }
        }
        else if (channel[i].getOperatorByFd(fd))
        {
            channel[i].removeOperatorByFd(fd);
            if (channel[i].getUserCount() == 0)
                channel.erase(channel.begin() + i);
            else
            {
                if (!channel[i].hasOperators())
                {
                    channel[i].promoteFirstUserToOperator();
                }
                reply = ":" + getClientByFd(fd)->getNickname() + "!~" + getClientByFd(fd)->getUser() + "@localhost QUIT " + reason + "\r\n";
                channel[i].broadcastMessage(reply);
            }
        }
    }
     // Notify user and perform cleanup
    notifyUsers(": leaving Server\r\n", fd);
    std::cout << "FD[" << fd << "] disconnected" << std::endl;
    clearEmptyChannels(fd);
    removeUserByFd(fd);
    removePollFd(fd);
    close(fd);
}

// // Extracts and formats the quit reason from the message
// std::string Server::quitReason(std::string message)
// {
//     std::istringstream stm(message);
//     std::string reason;
//     std::string str;

//     stm >> str;
//     quitReason2(message, str, reason);
//     if (reason.empty())
//         return (std::string("Quit"));
//     if (reason[0] != ':')
//     {
//         for (size_t i = 0; i < reason.size(); i++)
//         {
//             if (reason[i] == ' ') 
//             {
//                 reason.erase(i, reason.size() - i);
//                 break;
//             }
//         }
//         reason.insert(reason.begin(), ':');
//     }
//     return reason;
// }

// // Cette fonction extrait la raison de déconnexion d'un message et la formate correctement
// void Server::quitReason2(std::string message, std::string str, std::string &reason)
// {
//     size_t i = 0;

//     for (; i < message.size(); i++)
//     {
//         if (message[i] != ' ')
//         {
//             std::string tmp;
//             for (; i < message.size() && message[i] != ' '; i++)
//                 tmp += message[i];
//             if (tmp == str)
//                 break;
//             else
//                 tmp.clear();
//         }
//     }
//     if (i < message.size())
//         reason = message.substr(i);
//     i = 0;
//     for (; i < reason.size() && reason[i] == ' '; i++);
//     reason = reason.substr(i);
// }

// Extracts and formats the quit reason from the message
std::string Server::quitReason(const std::string &message) {
    size_t pos = message.find(" ");
    if (pos == std::string::npos) {
        return ":Quit"; // Default reason
    }

    std::string reason = message.substr(pos + 1);
    if (!reason.empty() && reason[0] != ':') {
        reason = ":" + reason;
    }
    return reason;
}