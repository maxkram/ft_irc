// #include "../../includes/server.hpp"

// // Fonction pour gérer la commande PASS
// void Server::PASS(std::string message, int fd)
// {
//     User *user;
//     std::string pass;

//     user = getClientByFd(fd);
//     std::string::iterator it = message.begin();
//     while (it != message.end() && (*it == ' ' || *it == '\t' || *it == '\v'))
//         ++it;
//     if (it != message.end() && *it == ':')
//         ++it;
//     message = std::string(it + 5, message.end());
//     if (message.empty())
//     {
//         notifyUsers(ERR_NOTENOUGHPARAMETERS(std::string("*")), fd);
//     }
//     else if (!user->isRegistered())
//     {
//         pass = message;
//         if (pass == password)
//             user->setRegistered(true);
//         else
//             notifyUsers(ERR_PASSWORDINCORECT(std::string("*")), fd);
//     }
//     else
//     {
//         notifyUsers(ERR_ALREADYREGISTERED(getClientByFd(fd)->getNickname()), fd);
//     }
// }

#include "../../includes/server.hpp"

// Handles the PASS command for setting the connection password
void Server::PASS(std::string message, int fd)
{
    // Get the user associated with the file descriptor
    
    User *user = getClientByFd(fd);
    if (!user) {
        notifyUsers(ERR_NOTREGISTERED("*"), fd); // User not found
        return;
    }
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
        std::string pass = message;
        // Validate the password
        if (pass == password)
            user->setRegistered(true);
        else
            notifyUsers(ERR_PASSWORDINCORECT(std::string("*")), fd);
    }
    else // Check if the user is already registered
    {
        notifyUsers(ERR_ALREADYREGISTERED(getClientByFd(fd)->getNickname()), fd);
    }
}
