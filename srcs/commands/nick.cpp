#include "../../includes/server.hpp"


void Server::NICK(std::string message, int fd)
{
    User *user = getClientByFd(fd);
    if (!user)
        return;
    // Trim leading whitespace and handle ':' prefix
    std::string::iterator it = message.begin();
    while (it != message.end() && (*it == ' ' || *it == '\t' || *it == '\v'))
        ++it;
    if (it != message.end() && *it == ':')
        ++it;
    // Extract the nickname    
    message = std::string(it + 5, message.end());
    if (message.empty())
    {
        notifyUsers(ERR_MISSING_PARAMETERS(std::string("*")), fd);
        return;
    }
    // Check if the nickname is already taken
    if (isNicknameTaken(message) && user->getNickname() != message)
    {
        if (user->getNickname().empty())
            notifyUsers(ERR_USER_NICKNAME_IN_USE(std::string(message), std::string(message)), fd);
        else
            notifyUsers(ERR_USER_NICKNAME_IN_USE(user->getNickname(), std::string(message)), fd);
        return;
    }
    if (!isValidNickname(message))
    {
        notifyUsers(ERR_INVALID_NICKNAME(std::string(message)), fd);
        return;
    }

    if (user && user->isRegistered())
    {
        std::string old = user->getNickname();
        user->setNickname(message);
        updateNicknameChannel(old, message);
        if (!old.empty() && old != message)
        {
            if (old == "*" && !user->getUser().empty())
            {
                user->setConnected(true);
                notifyUsers(RPL_SERVER_WELCOME(user->getNickname()), fd);
                notifyUsers(RPL_USER_NICK_CHANGE(user->getNickname(), message), fd);
            }
            else
                notifyUsers(RPL_USER_NICK_CHANGE(old, message), fd);
            return;
        }
    }
    else
        notifyUsers(ERR_USER_NOT_REGISTERED(message), fd);
    
    if (user && user->isRegistered() && !user->getUser().empty() && !user->getNickname().empty() && user->getNickname() != "*" && !user->isConnected())
    {
        user->setConnected(true);
        notifyUsers(RPL_SERVER_WELCOME(user->getNickname()), fd);
    }
}

void Server::updateNicknameChannel(std::string old, std::string n_nick)
{
    for (std::vector<Channel>::iterator ch = channel.begin(); ch != channel.end(); ++ch)
    {
        // Update nickname in the user list
        std::vector<User> &users = ch->getUsers();
        for (std::vector<User>::iterator us = users.begin(); us != users.end(); ++us)
        {
            if (us->getNickname() == old)
            {
                us->setNickname(n_nick);
                return;
            }
        }

        // Update nickname in the operators list
        std::vector<User> &operators = ch->getOperators();
        for (std::vector<User>::iterator us = operators.begin(); us != operators.end(); ++us)
        {
            if (us->getNickname() == old)
            {
                us->setNickname(n_nick);
                return;
            }
        }
    }
}

bool Server::isNicknameTaken(std::string &nickname)
{
    for (size_t i = 0; i < connectedUsers.size(); ++i)
    {
        if (connectedUsers[i].getNickname() == nickname)
            return true;
    }
    return false;
}

bool Server::isValidNickname(std::string &nickname)
{
    // Check if the first character is invalid
    if (nickname.empty() || nickname[0] == '&' || nickname[0] == '#' || nickname[0] == ':')
        return false;

    // Check the rest of the characters
    const std::string validSpecialChars = "-_[]{}\\`|";
    for (size_t i = 1; i < nickname.size(); ++i)
    {
        if (!std::isalnum(nickname[i]) && validSpecialChars.find(nickname[i]) == std::string::npos)
            return false;
    }
    return true;
}
