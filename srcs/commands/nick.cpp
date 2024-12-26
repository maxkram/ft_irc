#include "../../includes/server.hpp"

void Server::NICK(std::string message, int fd)
{
    User *user = getClientByFd(fd);
    if (!user) {
        return;
    }
    std::string old;
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
    
    if (isNicknameUsed(message) && user->getNickname() != message)
    {
        if (user->getNickname().empty())
            notifyUsers(ERR_NICKNAMEINUSE(std::string(message), std::string(message)), fd);
        else
            notifyUsers(ERR_NICKNAMEINUSE(user->getNickname(), std::string(message)), fd);
        return;
    }

    if (!validNickname(message))
    {
        notifyUsers(ERR_ERRONEUSNICKNAME(std::string(message)), fd);
        return;
    }

    if (user->isRegistered())
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
    } else {
        notifyUsers(ERR_NOTREGISTERED(message), fd);
        return; // ????????????????????? NOT SURE ???????????????????????????
    }

    if (user && user->isRegistered() && !user->getUser().empty() && !user->getNickname().empty() && user->getNickname() != "*" && !user->isConnected())
    {
        user->setConnected(true);
        notifyUsers(RPL_WELCOME(user->getNickname()), fd);
    }
}

void Server::updateNicknameChannel(std::string old, std::string n_nick)
{
    for (std::vector<Channel>::iterator ch = channel.begin(); ch != channel.end(); ++ch) {
        // Update nickname in the list of users
        std::vector<User> &users = (*ch).getUsers();
        for (std::vector<User>::iterator us = users.begin(); us != users.end(); ++us) {
            if (us->getNickname() == old) {
                us->setNickname(n_nick);
                return;
            }
        }

        // Update nickname in the list of operators
        std::vector<User> &operators = (*ch).getOperators();
        for (std::vector<User>::iterator us = operators.begin(); us != operators.end(); ++us) {
            if (us->getNickname() == old) {
                us->setNickname(n_nick);
                return;
            }
        }
    }
}

bool Server::isNicknameUsed(std::string &nickname)
{
    for (size_t i = 0; i < this->sock_user.size(); ++i)
    {
        if (this->sock_user[i].getNickname() == nickname)
            return true;
    }
    return false;
}

bool Server::validNickname(std::string &nickname) {
    // Check if the first character is invalid
    if (nickname.empty() || nickname[0] == '&' || nickname[0] == '#' || nickname[0] == ':') {
        return false;
    }

    // Allowed special characters
    const std::string allowedSpecialChars = "-_[]{}\\`|";

    // Validate remaining characters
    for (size_t i = 1; i < nickname.size(); ++i) {
        if (!std::isalnum(nickname[i]) && allowedSpecialChars.find(nickname[i]) == std::string::npos) {
            return false;
        }
    }

    return true;
}
