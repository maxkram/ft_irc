#include "../../includes/server.hpp"

void Server::PING(std::string &message, int fd) {
    User *user = getClientByFd(fd);
    if (!user) {
        return;
    }

    // Extract parameters from the message
    std::vector<std::string> param = extractParams(message);

    // Check if there are enough parameters
    if (param.size() < 2) {
        notifyUsers(ERR_NOTENOUGHPARAMETERS(user->getNickname()), fd);
        return;
    }

    // Respond with PONG
    notifyUsers(RPL_PONG, fd);
}
