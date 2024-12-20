#include "../../includes/server.hpp"

// Fonction pour gérer la commande PASS
void Server::PASS(std::string message, int fd)
{
    User *user;
    std::string pass;

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
    }
    else if (!user->isRegistered())
    {
        pass = message;
        if (pass == password)
            user->setRegistered(true);
        else
            notifyUsers(ERR_PASSWORDINCORECT(std::string("*")), fd);
    }
    else
    {
        notifyUsers(ERR_ALREADYREGISTERED(getClientByFd(fd)->getNickname()), fd);
    }
}
