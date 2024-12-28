#include "../../includes/server.hpp"

void Server::QUIT(std::string message, int fd)
{
    std::string reason;
    std::string reply;

    reason = extractQuitReason(message);
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
                    channel[i].promoteFirstUser();
                }
                reply = ":" + getClientByFd(fd)->getNickname() + "!~" + getClientByFd(fd)->getUser() + "@localhost QUIT " + reason + "\r\n";
                channel[i].broadcastMessage(reply);
            }
        }
    }
    notifyUsers(": leaving Server\r\n", fd);
    std::cout << "FD[" << fd << "] disconnected" << std::endl;
    clearChannel(fd);
    removeClient(fd);
    removePollFd(fd);
    close(fd);
}

std::string Server::extractQuitReason(std::string message)
{
    std::istringstream stm(message);
    std::string reason;
    std::string str;

    stm >> str;
    appendQuitReason(message, str, reason);
    if (reason.empty())
        return (std::string("Quit"));
    if (reason[0] != ':')
    {
        for (size_t i = 0; i < reason.size(); i++)
        {
            if (reason[i] == ' ') 
            {
                reason.erase(i, reason.size() - i);
                break;
            }
        }
        reason.insert(reason.begin(), ':');
    }
    return reason;
}

void Server::appendQuitReason(std::string message, std::string str, std::string &reason)
{
    size_t i = 0;

    for (; i < message.size(); i++)
    {
        if (message[i] != ' ')
        {
            std::string tmp;
            for (; i < message.size() && message[i] != ' '; i++)
                tmp += message[i];
            if (tmp == str)
                break;
            else
                tmp.clear();
        }
    }
    if (i < message.size())
        reason = message.substr(i);
    i = 0;
    for (; i < reason.size() && reason[i] == ' '; i++);
    reason = reason.substr(i);
}