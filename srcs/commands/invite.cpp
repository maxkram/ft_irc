#include "../../includes/server.hpp"

void Server::INVITE(std::string message, int fd)
{
    // Parse the message to extract parameters
    std::vector<std::string> params = extractParams(message);
    User *user = getClientByFd(fd);

    // Check if the number of parameters is sufficient
    if (params.size() < 2)
    {
        notifyUsers(ERR_NOTENOUGHPARAMETERS(user->getNickname()), fd);
        return;
    }

    // Extract user and channel names from parameters
    std::string invitedUserName = params[1];
    std::string channelName = params[2].substr(1);

    // Check if the channel exists
    Channel *channel = getChannel(channelName);
    if (!channel)
    {
        notifyClient2(403, user->getNickname(), params[2], fd, " :No such channel\r\n");
        return;
    }

    // Check if the user is a member of the channel
    if (!channel->isUserInChannel(user->getNickname()))
    {
        notifyClient2(442, user->getNickname(), channelName, fd, " :You're not on that channel\r\n");
        return;
    }

    // Check if the user has operator privileges in the channel
    if (!channel->isUserOperator(user->getNickname()))
    {
        notifyClient2(482, user->getNickname(), channelName, fd, " :You're not a channel operator\r\n");
        return;
    }

    // Check if the invited user is already in the channel
    if (channel->isUserInChannel(invitedUserName))
    {
        notifyUsers(ERR_USERONCHANNEL(user->getNickname(), invitedUserName, channelName), fd);
        return;
    }

    // Check if the invited user exists
    User *invitedUser = getClientByNickname(invitedUserName);
    if (invitedUser)
    {
        // Add an invitation to the invited user's invitation list
        invitedUser->addInvitation(channelName);

        // Notify the invited user
        std::string notification = user->getNickname() + " has invited you to the channel #" + channelName + "\r\n";
        notifyUsers(notification, invitedUser->getFduser());
    }
    else
    {
        // Notify the inviting user that the invited user does not exist
        notifyClient2(401, user->getNickname(), invitedUserName, fd, " :No such nick\r\n");
    }
}
