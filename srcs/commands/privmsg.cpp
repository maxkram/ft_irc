#include "../../includes/server.hpp"

// Fonction pour obtenir le nom du canal cible
std::string Server::getChannelTarget(std::string &target)
{
    Channel *ch = NULL;
    std::string ch_name;

    ch = getChannel(target);
    if (ch != NULL && ch->getChannelName() == target)
        return (ch->getChannelName());
    return (std::string(""));
}

// Fonction pour obtenir le nom d'utilisateur cible à partir du descripteur de fichier
std::string Server::getUserTarget(int fd)
{
    User *us = NULL;

    us = getClientByFd(fd);
    return (us == NULL ? std::string("") : us->getNickname());
}

// Fonction pour analyser la commande PRIVMSG et gérer les erreurs
int Server::handlePrivmsg(std::string split_message[3], std::string split_params[3], int fd)
{
    switch (validatePrivmsgSyntax(split_message, split_params))
    {
        case 0:
            break;
        case 1:
            notifyUsers(ERR_NORECIPIENT(split_message[0]), fd);
            return (-1);
        case 2:
            notifyUsers(ERR_NOTEXTTOSEND(), fd);
            return (-1);
        case 3:
            notifyUsers(ERR_NOTENOUGHPARAMETERS(getClientByFd(fd)->getNickname()), fd);
            return (-1);
        case 4:
            notifyUsers(ERR_NOTOPLEVEL(split_params[0]), fd);
            return (-1);
        case 5:
            notifyUsers(ERR_WILDCARDTOPLEVEL(split_params[0]), fd);
            return (-1);
        case 6:
            notifyUsers(ERR_TOOMANYRECIPIENTS(split_params[0]), fd);
            return (-1);
        default:
            break;
    }

    std::string tmp("");
    if (split_params[0][0] == '&' || split_params[0][0] == '#')
    {
        std::string tmp_ch_name(&split_params[0][1]);
        tmp = getChannelTarget(tmp_ch_name);
        if (tmp.empty() == 1 || getChannel(&split_params[0][1]) == NULL)
        {
            notifyUsers(ERR_NOSUCHNICK(split_params[0]), fd);
            return (1);
        }
    }
    else
    {
        tmp = getUserTarget(fd);
        if (tmp.empty() == 1 || getClientByFd(fd) == NULL || getClientByNickname(split_params[0]) == NULL)
        {
            notifyUsers(ERR_NOSUCHNICK(tmp), fd);
            return (1);
        }
    }
    return (0);
}

// Fonction pour traiter la commande PRIVMSG (envoi de message privé)
void Server::PRIVMSG(std::string &message, int fd)
{
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
        Channel *channel = getChannel(&split_params[0][1]);
        if (!channel->isUserInChannel(getClientByFd(fd)->getNickname()))
        {
            notifyClient2(442, getClientByFd(fd)->getNickname(),  channel->getChannelName(), getClientByFd(fd)->getFduser(), " :You're not on that channel\r\n");
            return;
        }
        std::vector<User> ch_usrs = getChannel(&split_params[0][1])->getUsers();
        for (size_t i = 0; i < ch_usrs.size(); i++)
        {
            if (getClientByFd(fd)->getHostname() != ch_usrs[i].getHostname())
            {
                notifyUsers(RPL_PRIVMSGCHANNEL(getClientByFd(fd)->getHostname(), getClientByFd(fd)->getIp(), getChannel(&split_params[0][1])->getChannelName(), split_params[1]), ch_usrs[i].getFduser());
            }
        }
        ch_usrs = getChannel(&split_params[0][1])->getOperators();
        for (size_t i = 0; i < ch_usrs.size(); i++)
        {
            if (getClientByFd(fd)->getHostname() != ch_usrs[i].getHostname())
            {
                notifyUsers(RPL_PRIVMSGCHANNEL(getClientByFd(fd)->getHostname(), getClientByFd(fd)->getIp(), getChannel(&split_params[0][1])->getChannelName(), split_params[1]), ch_usrs[i].getFduser());
            }
        }
    }
    else
    {
        if (getClientByFd(fd)->getHostname() != getClientByNickname(split_params[0])->getHostname())
        {
            notifyUsers(RPL_PRIVMSGUSER(getClientByFd(fd)->getHostname(), getClientByFd(fd)->getIp(), getClientByNickname(split_params[0])->getNickname(), split_params[1]), getClientByNickname(split_params[0])->getFduser());
        }
    }
}
