#include "../../includes/server.hpp"


void Server::NICK(std::string message, int fd)
{
    User *user;
    std::string old;
    
    user = getClientByFd(fd);
    std::string::iterator it = message.begin();
    while (it != message.end() && (*it == ' ' || *it == '\t' || *it == '\v'))
        ++it;
    if (it != message.end() && *it == ':')
        ++it;
    message = std::string(it + 5, message.end());
    if (message.empty())
    {
        notifyUsers(ERR_NOTENOUGHPARAMETERS(std::string("*")), fd);
        return;
    }
    if (isNicknameTaken(message) && user->getNickname() != message)
    {
        if (user->getNickname().empty())
            notifyUsers(ERR_NICKNAMEINUSE(std::string(message), std::string(message)), fd);
        else
            notifyUsers(ERR_NICKNAMEINUSE(user->getNickname(), std::string(message)), fd);
        return;
    }
    if (!isValidNickname(message))
    {
        notifyUsers(ERR_ERRONEUSNICKNAME(std::string(message)), fd);
        return;
    }
    else
    {
        if (user && user->isRegistered())
        {
            old = user->getNickname();
            user->setNickname(message);
            updateNicknameChannel(old, message);
            if (!old.empty() && old != message)
            {
                if (old == "*" && !user->getUser().empty())
                {
                    user->setConnected(true);
                    notifyUsers(RPL_WELCOME(user->getNickname()), fd);
                    notifyUsers(RPL_CHANGENICK(user->getNickname(), message), fd);
                }
                else
                    notifyUsers(RPL_CHANGENICK(old, message), fd);
                return;
            }
        }
        else if (user && !user->isRegistered())
            notifyUsers(ERR_NOTREGISTERED(message), fd);
    }
    if (user && user->isRegistered() && !user->getUser().empty() && !user->getNickname().empty() && user->getNickname() != "*" && !user->isConnected())
    {
        user->setConnected(true);
        notifyUsers(RPL_WELCOME(user->getNickname()), fd);
    }
}

void Server::updateNicknameChannel(std::string old, std::string n_nick)
{
    for (std::vector<Channel>::iterator ch = channel.begin(); ch < channel.end(); ch++)
    {
        for (std::vector<User>::iterator us = (*ch).getUsers().begin(); us < (*ch).getUsers().end(); us++)
        {
            if (us->getNickname() == old)
            {
                us->setNickname(n_nick);
                return;
            }
        }
        for (std::vector<User>::iterator us = (*ch).getOperators().begin(); us < (*ch).getOperators().end(); us++)
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
    for (size_t i = 0; i < this->connectedUsers.size(); i++)
    {
        if (this->connectedUsers[i].getNickname() == nickname)
            return true;
    }
    return false;
}

bool Server::isValidNickname(std::string &nickname)
{
    if (nickname[0] == '&' || nickname[0] == '#' || nickname[0] == ':')
        return false;
    for (size_t i = 1; i < nickname.size(); i++)
    {
        if (!std::isalnum(nickname[i]) && (std::string("-_[]{}\\`|").find(nickname[i]) == std::string::npos))
            return false;
    }
    return true;
}