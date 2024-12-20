// #include "../../includes/server.hpp"

// // Fonction pour gérer la commande PING
// void Server::PING(std::string &message, int fd)
// {
//     User *user;
//     std::vector<std::string> param;

//     user = getClientByFd(fd);
//     param = extractParams(message);
//     if (user && param.size() < 2)
//     {
//         notifyUsers(ERR_NOTENOUGHPARAMETERS(user->getNickname()), fd);
//         return;
//     }
//     notifyUsers(RPL_PONG, fd);
// }


#include "../../includes/server.hpp"

// Handles the PING command to respond with a PONG
void Server::PING(std::string &message, int fd)
{
    User *user = getClientByFd(fd);
    if (!user) {
        notifyUsers(ERR_NOTREGISTERED("*"), fd); // If user is not found
        return;
    }
    // Extract parameters from the message
    std::vector<std::string> param = extractParams(message);
    // Check if enough parameters are provided
    if (user && param.size() < 2)
    {
        notifyUsers(ERR_NOTENOUGHPARAMETERS(user->getNickname()), fd);
        return;
    }
    notifyUsers(RPL_PONG, fd);
}
