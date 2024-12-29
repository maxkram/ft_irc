#include "../../includes/server.hpp"

void Server::PING(std::string &message, int fd)
{
    User *user = getClientByFd(fd);
    std::vector<std::string> params = extractParams(message);

    if (user && params.size() < 2)
    {
        notifyUsers(ERR_NOTENOUGHPARAMETERS(user->getNickname()), fd);
        return;
    }

    notifyUsers(RPL_PONG, fd);
}
