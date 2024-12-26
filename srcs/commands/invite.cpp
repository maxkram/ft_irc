#include "../../includes/server.hpp"

void Server::INVITE(std::string message, int fd)
{
    std::vector<std::string> param = extractParams(message);
    User *user = getClientByFd(fd);

    // Validate the user issuing the invite
    if (!user)
    {
        std::cerr << "Error: Invalid user issuing the INVITE." << std::endl;
        return;
    }

    if (param.size() < 2) {
        notifyUsers(ERR_NOTENOUGHPARAMETERS(user->getNickname()), fd);
        return;
    }

    std::string invitedUserName = param[1];
    std::string channelName = param[2].substr(1);

    if (!isChannelAvailable(param[2])) {
        notifyClient2(403, getClientByFd(fd)->getNickname(), param[2], getClientByFd(fd)->getFduser(), " :No such channel\r\n");
        return;
    }

    Channel *channel = getChannel(channelName);
    if (!channel->isUserInChannel(user->getNickname())) {
        notifyClient2(442, getClientByFd(fd)->getNickname(), channelName, getClientByFd(fd)->getFduser(), " :You're not on that channel\r\n");
        return;
    }
    if (!channel->isUserOperator(user->getNickname())) {
        notifyClient2(482, getClientByFd(fd)->getNickname(), channelName, getClientByFd(fd)->getFduser(), " :You're not channel operator\r\n");
        return;
    }
    if (channel->isUserInChannel(invitedUserName)) {
        notifyUsers(ERR_USERONCHANNEL(user->getNickname(), invitedUserName, channelName), fd);
        return;
    }
    User *invitedUser = getClientByNickname(invitedUserName);
    if (invitedUser) {
        invitedUser->addInvitation(channelName);
        std::string notif;
        notif = getClientByFd(fd)->getNickname() + " has invited you to the channel #" + channelName + "\r\n";
        notifyUsers(notif, invitedUser->getFduser());
        
    }
    else {
        notifyClient2(401, user->getNickname(), invitedUserName, user->getFduser(), " :No such nick\r\n");
    }
}
