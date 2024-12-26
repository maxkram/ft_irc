#include "../../includes/server.hpp"

void Server::PASS(std::string message, int fd) {
    User *user = getClientByFd(fd);
    if (!user) {
        return;
    }

    // Skip leading whitespace and extract the password
    std::string::iterator it = message.begin();
    while (it != message.end() && (*it == ' ' || *it == '\t' || *it == '\v')) {
        ++it;
    }
    if (it != message.end() && *it == ':') {
        ++it;
    }

    // Extract the password part from the message
    message = std::string(it + 5, message.end());

    // Handle empty password case
    if (message.empty()) {
        notifyUsers(ERR_NOTENOUGHPARAMETERS(std::string("*")), fd);
        return;
    }

    // Handle unregistered user
    if (!user->isRegistered()) {
        if (message == password) {
            user->setRegistered(true);
        } else {
            notifyUsers(ERR_PASSWORDINCORECT(std::string("*")), fd);
        }
        return;
    }

    // Handle already registered user
    notifyUsers(ERR_ALREADYREGISTERED(user->getNickname()), fd);
}
