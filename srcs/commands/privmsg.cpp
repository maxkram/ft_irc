#include "../../includes/server.hpp"

std::string Server::getChannelTarget(std::string &target)
{
    Channel *channel = getChannel(target);
    if (channel != NULL)
    {
        const std::string &channelName = channel->getChannelName();
        if (channelName == target)
            return channelName;
    }
    return std::string("");
}

std::string Server::getUserTarget(int fd)
{
    User *us = getClientByFd(fd);
    return (us == NULL ? std::string("") : us->getNickname());
}

int Server::handlePrivmsg(std::string split_message[3], std::string split_params[3], int fd)
{
    int validationResult = validatePrivmsgSyntax(split_message, split_params);
    switch (validationResult)
    {
        case 0:
            break;
        case 1:
            notifyUsers(ERR_NORECIPIENT(split_message[0]), fd);
            return -1;
        case 2:
            notifyUsers(ERR_NOTEXTTOSEND(), fd);
            return -1;
        case 3:
            notifyUsers(ERR_NOTENOUGHPARAMETERS(getClientByFd(fd)->getNickname()), fd);
            return -1;
        case 4:
            notifyUsers(ERR_NOTOPLEVEL(split_params[0]), fd);
            return -1;
        case 5:
            notifyUsers(ERR_WILDCARDTOPLEVEL(split_params[0]), fd);
            return -1;
        case 6:
            notifyUsers(ERR_TOOMANYRECIPIENTS(split_params[0]), fd);
            return -1;
        default:
            break;
    }

    std::string tmp;
    if (split_params[0][0] == '&' || split_params[0][0] == '#')
    {
        std::string channelName = split_params[0].substr(1);
        tmp = getChannelTarget(channelName);
        if (tmp.empty() || getChannel(channelName.c_str()) == NULL)
        {
            notifyUsers(ERR_NOSUCHNICK(split_params[0]), fd);
            return 1;
        }
    }
    else
    {
        tmp = getUserTarget(fd);
        if (tmp.empty() || getClientByFd(fd) == NULL || getClientByNickname(split_params[0]) == NULL)
        {
            notifyUsers(ERR_NOSUCHNICK(tmp), fd);
            return 1;
        }
    }
    return 0;
}

void Server::PRIVMSG(std::string &message, int fd)
{
    User *client = getClientByFd(fd);
    std::string split_message[3] = {std::string(), std::string(), std::string()};
    if (splitMessage(message, split_message))
        return;

    std::string split_params[3] = {std::string(), std::string(), std::string()};
    if (splitParams(split_message[2], split_params) == 1)
        return;

    if (handlePrivmsg(split_message, split_params, fd))
        return;

    if (split_params[0][0] == '&' || split_params[0][0] == '#')
    {
        std::string channelName = split_params[0].substr(1);
        Channel *channel = getChannel(channelName.c_str());

        if (!channel || !channel->isUserInChannel(client->getNickname()))
        {
            sendErrorToClient(442, client->getNickname(), channelName, client->getFduser(), " :You're not on that channel\r\n");
            return;
        }

        std::vector<User> channelUsers = channel->getUsers();
        for (size_t i = 0; i < channelUsers.size(); ++i)
        {
            if (client->getHostname() != channelUsers[i].getHostname())
            {
                notifyUsers(
                    RPL_PRIVMSGCHANNEL(
                        client->getHostname(),
                        client->getIp(),
                        channel->getChannelName(),
                        split_params[1]
                    ),
                    channelUsers[i].getFduser()
                );
            }
        }

        std::vector<User> channelOperators = channel->getOperators();
        for (size_t i = 0; i < channelOperators.size(); ++i)
        {
            if (client->getHostname() != channelOperators[i].getHostname())
            {
                notifyUsers(
                    RPL_PRIVMSGCHANNEL(
                        client->getHostname(),
                        client->getIp(),
                        channel->getChannelName(),
                        split_params[1]
                    ),
                    channelOperators[i].getFduser()
                );
            }
        }
    }
    else
    {
        User *recipient = getClientByNickname(split_params[0]);
        if (recipient && client->getHostname() != recipient->getHostname())
        {
            notifyUsers(
                RPL_PRIVMSGUSER(
                    client->getHostname(),
                    client->getIp(),
                    recipient->getNickname(),
                    split_params[1]
                ),
                recipient->getFduser()
            );
        }
    }
}
