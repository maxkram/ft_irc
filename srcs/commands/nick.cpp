#include "../../includes/server.hpp"


// Function to manage the NICK command (nickname change);
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

// Function to update the user name on all channels where it is present
// void Server::updateNicknameChannel(std::string old, std::string n_nick)
// {
//     for (std::vector<Channel>::iterator ch = channel.begin(); ch < channel.end(); ch++)
//     {
//         for (std::vector<User>::iterator us = (*ch).getUsers().begin(); us < (*ch).getUsers().end(); us++)
//         {
//             if (us->getNickname() == old)
//             {
//                 us->setNickname(n_nick);
//                 return;
//             }
//         }
//         for (std::vector<User>::iterator us = (*ch).getOperators().begin(); us < (*ch).getOperators().end(); us++)
//         {
//             if (us->getNickname() == old)
//             {
//                 us->setNickname(n_nick);
//                 return;
//             }        
//         }
//     }
// }

// Update the nickname in all channels where the user is present
void Server::updateNicknameChannel(std::string& oldNickname, const std::string& newNickname) {
    for (std::vector<Channel>::iterator ch = channel.begin(); ch != channel.end(); ++ch) {
        if (updateNicknameInList(ch->getUsers(), oldNickname, newNickname)) {
            return; // Found and updated in users
        }
        if (updateNicknameInList(ch->getOperators(), oldNickname, newNickname)) {
            return; // Found and updated in operators
        }
    }
}

// Update the nickname in a user list
bool Server::updateNicknameInList(std::vector<User>& users, const std::string& oldNickname, const std::string& newNickname) {
    for (std::vector<User>::iterator us = users.begin(); us != users.end(); ++us) {
        if (us->getNickname() == oldNickname) {
            us->setNickname(newNickname);
            return true;
        }
    }
    return false;
}

// Function to check if the nickname is already used by another user
// bool Server::isNicknameUsed(std::string &nickname)
// {
//     for (size_t i = 0; i < this->sock_user.size(); i++)
//     {
//         if (this->sock_user[i].getNickname() == nickname)
//             return true;
//     }
//     return false;
// }

// Check if the nickname is already used by another user
bool Server::isNicknameUsed(const std::string& nickname) {
    for (std::vector<User>::iterator it = this->sock_user.begin(); it != this->sock_user.end(); ++it) {
        if (it->getNickname() == nickname) {
            return true;
        }
    }
    return false;
}

// // Fonction pour vérifier si le nickname est valide
// bool Server::validNickname(std::string &nickname)
// {
//     if (nickname[0] == '&' || nickname[0] == '#' || nickname[0] == ':')
//         return false;
//     for (size_t i = 1; i < nickname.size(); i++)
//     {
//         if (!std::isalnum(nickname[i]) && (std::string("-_[]{}\\`|").find(nickname[i]) == std::string::npos))
//             return false;
//     }
//     return true;
// }

// Validate the format of the nickname
bool Server::validNickname(std::string& nickname) {
    if (nickname.empty() || nickname[0] == '&' || nickname[0] == '#' || nickname[0] == ':') {
        return false;
    }
    for (size_t i = 0; i < nickname.size(); ++i) {
        if (!std::isalnum(nickname[i]) && std::string("-_[]{}\\`|").find(nickname[i]) == std::string::npos) {
            return false;
        }
    }
    return true;
}