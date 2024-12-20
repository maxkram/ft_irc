#include "../../includes/server.hpp"

// Fonction pour gérer la commande PING
void Server::PING(std::string &message, int fd)
{
    User *user;
    std::vector<std::string> param;

    user = getClientByFd(fd);
    param = extractParams(message);
    if (user && param.size() < 2)
    {
        notifyUsers(ERR_NOTENOUGHPARAMETERS(user->getNickname()), fd);
        return;
    }
    notifyUsers(RPL_PONG, fd);
}
