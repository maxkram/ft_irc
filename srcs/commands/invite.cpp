#include "../../includes/server.hpp"

void Server::INVITE(std::string message, int fd)
{
    std::vector<std::string> param = extractParams(message);
    User *user = getClientByFd(fd);

    if (param.size() < 2)
    {
        notifyUsers(ERR_NOTENOUGHPARAMETERS(user->getNickname()), fd);
        return;
    }

    std::string invitedUserName = param[1];
    std::string channelName = param[2].substr(1);

    // Validate channel existence
    if (!isChannelAvailable(param[2]))
    {
        sendErrorToClient(403, user->getNickname(), param[2], user->getFduser(), " :No such channel\r\n");
        return;
    }

    Channel *channel = getChannel(channelName);

    // Check user membership in the channel
    if (!channel->isUserInChannel(user->getNickname()))
    {
        sendErrorToClient(442, user->getNickname(), channelName, user->getFduser(), " :You're not on that channel\r\n");
        return;
    }

    // Check if the user is a channel operator
    if (!channel->isUserOperator(user->getNickname()))
    {
        sendErrorToClient(482, user->getNickname(), channelName, user->getFduser(), " :You're not channel operator\r\n");
        return;
    }

    // Check if the invited user is already in the channel
    if (channel->isUserInChannel(invitedUserName))
    {
        notifyUsers(ERR_USERONCHANNEL(user->getNickname(), invitedUserName, channelName), fd);
        return;
    }

    // Handle inviting the user
    User *invitedUser = getClientByNickname(invitedUserName);
    if (invitedUser != NULL)
    {
        invitedUser->addInvitation(channelName);

        // Notify the invited user
        std::string notif = user->getNickname() + " has invited you to the channel #" + channelName + "\r\n";
        notifyUsers(notif, invitedUser->getFduser());
    }
    else
    {
        sendErrorToClient(401, user->getNickname(), invitedUserName, user->getFduser(), " :No such nick\r\n");
    }
}
