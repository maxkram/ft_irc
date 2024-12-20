#include "../../includes/server.hpp"

// Méthode pour gérer la commande INVITE
void Server::INVITE(std::string message, int fd)
{
    std::vector<std::string> param;
    User *user;
    Channel *channel;
    std::string invitedUserName;
    std::string channelName;

    param = extractParams(message);
    user = getClientByFd(fd);

    if (param.size() < 2)
    {
        notifyUsers(ERR_NOTENOUGHPARAMETERS(user->getNickname()), fd);
        return;
    }
    if (!isChannelAvailable(param[2]))
    {
        notifyClient2(403, getClientByFd(fd)->getNickname(), param[2], getClientByFd(fd)->getFduser(), " :No such channel\r\n");
        return;
    }

    invitedUserName = param[1];
    channelName = param[2].substr(1);

    channel = getChannel(channelName);
    if (!channel->isUserInChannel(user->getNickname()))
    {
        notifyClient2(442, getClientByFd(fd)->getNickname(), channelName, getClientByFd(fd)->getFduser(), " :You're not on that channel\r\n");
        return;
    }
    if (!channel->isUserOperator(user->getNickname()))
    {
        notifyClient2(482, getClientByFd(fd)->getNickname(), channelName, getClientByFd(fd)->getFduser(), " :You're not channel operator\r\n");
        return;
    }
    if (channel->isUserInChannel(invitedUserName))
    {
        notifyUsers(ERR_USERONCHANNEL(user->getNickname(), invitedUserName, channelName), fd);
        return;
    }
    User *invitedUser = getClientByNickname(invitedUserName);
    if (invitedUser != NULL)
    {
        invitedUser->addInvitation(channelName);
        std::string notif;
        notif = getClientByFd(fd)->getNickname() + " has invited you to the channel #" + channelName + "\r\n";
        notifyUsers(notif, invitedUser->getFduser());
        
    }
    else
    {
        notifyClient2(401, user->getNickname(), invitedUserName, user->getFduser(), " :No such nick\r\n");
    }
}
