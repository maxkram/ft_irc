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
    std::istringstream stream(message);
    std::string reason;
    std::string firstWord;

    // Extract the first word from the message
    stream >> firstWord;

    // Append the remaining message as the reason
    appendQuitReason(message, firstWord, reason);

    // Default to "Quit" if no reason is provided
    if (reason.empty())
    {
        return "Quit";
    }

    // Ensure the reason starts with ':'
    if (reason[0] != ':')
    {
        // Trim the reason at the first space
        for (size_t i = 0; i < reason.size(); ++i)
        {
            if (reason[i] == ' ')
            {
                reason.erase(i);
                break;
            }
        }
        reason.insert(reason.begin(), ':'); // Prepend ':'
    }

    return reason;
}

void Server::appendQuitReason(std::string message, std::string str, std::string &reason)
{
    size_t i = 0;

    // Find the position of the first occurrence of `str` in the message
    while (i < message.size())
    {
        // Skip leading spaces
        if (message[i] != ' ')
        {
            std::string tmp;
            // Collect the word
            while (i < message.size() && message[i] != ' ')
                tmp += message[i++];
            
            // Check if the word matches the target string
            if (tmp == str)
                break;
        }
        else
        {
            ++i; // Move to the next character
        }
    }

    // Extract the reason starting from the position after `str`
    if (i < message.size())
        reason = message.substr(i);

    // Remove leading spaces from the reason
    size_t start = 0;
    while (start < reason.size() && reason[start] == ' ')
        ++start;

    reason = reason.substr(start);
}
