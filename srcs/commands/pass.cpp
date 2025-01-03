#include "../../includes/server.hpp"

void Server::PASS(std::string message, int fd)
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

    // Extract the password
    std::string pass(it + 5, message.end());

    if (pass.empty())
    {
        notifyUsers(ERR_MISSING_PARAMETERS(std::string("*")), fd);
    }
    else if (!user->isRegistered())
    {
        if (pass == password)
        {
            user->setRegistered(true);
        }
        else
        {
            notifyUsers(ERR_PASSWORD_INCORRECT(std::string("*")), fd);
        }
    }
    else
    {
        notifyUsers(ERR_ALREADY_REGISTERED(user->getNickname()), fd);
    }
}
