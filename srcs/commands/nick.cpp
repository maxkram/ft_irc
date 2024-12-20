#include "../../includes/server.hpp"


// Fonction pour gérer la commande NICK (changement de nickname)
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

// Fonction pour mettre à jour le nickname de l'utilisateur dans tous les canaux où il est présent
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

// Fonction pour vérifier si le nickname est déjà utilisé par un autre utilisateur
bool Server::isNicknameUsed(std::string &nickname)
{
    for (size_t i = 0; i < this->sock_user.size(); i++)
    {
        if (this->sock_user[i].getNickname() == nickname)
            return true;
    }
    return false;
}

// Fonction pour vérifier si le nickname est valide
bool Server::validNickname(std::string &nickname)
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
