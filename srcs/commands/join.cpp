#include "../../includes/server.hpp"

// Méthode pour gérer la commande JOIN
void Server::JOIN(std::string message, int fd)
{
    std::vector<std::pair<std::string, std::string> > param;
    bool flag;

    if (!splitJoinParams(param, message, fd))
    {
        notifyClient3(461, getClientByFd(fd)->getNickname(), getClientByFd(fd)->getFduser(), " :Not enough parameters\r\n");
        return;
    }
    if (param.size() > 20)
    {
        notifyClient3(407, getClientByFd(fd)->getNickname(), getClientByFd(fd)->getFduser(), " :Too many channels\r\n");
        return;
    }
    for (size_t i = 0; i < param.size(); i++)
    {
        flag = false;
        for (size_t j = 0; j < this->channel.size(); j++)
        {
            if (this->channel[j].getChannelName() == param[i].first)
            {
                addClientToExistChannel(param, i, j, fd);
                flag = true;
                break;
            }
        }
        if (!flag)
            createAndAddToNewChannel(param, i, fd);
    }
}

// Méthode pour diviser le message JOIN en noms de canaux et clés
int Server::splitJoinParams(std::vector<std::pair<std::string, std::string> > &param, std::string message, int fd)
{
    std::vector<std::string> vec;
    std::istringstream ss(message);
    std::string line;
    std::string channel;
    std::string key;
    std::string str;

    while (std::getline(ss, line, ' '))
        vec.push_back(line);

    if (vec.size() < 2)
    {
        param.clear();
        return 0;
    }
    vec.erase(vec.begin());
    channel = vec[0];
    vec.erase(vec.begin());
    if (!vec.empty())
    {
        key = vec[0];
        vec.clear();
    }
    for (size_t i = 0; i < channel.size(); i++)
    {
        if (channel[i] == ',')
        {
            param.push_back(std::make_pair(str, ""));
            str.clear();
        }
        else
            str += channel[i];
    }
    param.push_back(std::make_pair(str, ""));
    if (!key.empty())
    {
        size_t j = 0;
        str.clear();
        for (size_t i = 0; i < key.size(); i++)
        {
            if (key[i] == ',')
            {
                param[j].second = str;
                j++;
                str.clear();
            }
            else
                str += key[i];
        }
        param[j].second = str;
    }
    for (size_t i = 0; i < param.size(); i++)
    {
        if (param[i].first.empty())
            param.erase(param.begin() + i--);
    }
    for (size_t i = 0; i < param.size(); i++)
    {
        if (*(param[i].first.begin()) != '#')
        {
            notifyClient2(403, getClientByFd(fd)->getNickname(), param[i].first, getClientByFd(fd)->getFduser(), " :No such channel\r\n");
            param.erase(param.begin() + i--);
        }
        else
            param[i].first.erase(param[i].first.begin());
    }
    return 1;
}

// Ajouter un utilisateur dans un canal existant
void Server::addClientToExistChannel(std::vector<std::pair<std::string, std::string> > &param, int i, int j, int fd)
{
    if (this->channel[j].getFindUserByName(getClientByFd(fd)->getNickname()))
        return;
    if (channelUserCount(getClientByFd(fd)->getNickname()) >= 20)
    {
        notifyClient3(405, getClientByFd(fd)->getNickname(), getClientByFd(fd)->getFduser(), " :You have joined too many channels\r\n");
        return;
    }
    if (!this->channel[j].getChannelPassword().empty() && this->channel[j].getChannelPassword() != param[i].second)
    {
        if (!isUserInvited(getClientByFd(fd), param[i].first, 0))
        {
            notifyClient2(475, getClientByFd(fd)->getNickname(), "#" + param[i].first, getClientByFd(fd)->getFduser(), " :Cannot join channel (+k) - bad key\r\n");
            return;
        }
    }
    if (this->channel[j].getInviteOnlyStatus())
    {
        if (!isUserInvited(getClientByFd(fd), param[i].first, 1))
        {
            notifyClient2(473, getClientByFd(fd)->getNickname(), "#" + param[i].first, getClientByFd(fd)->getFduser(), " :Cannot join channel (+i)\r\n");
            return;
        }
    }
    if (this->channel[j].getUserLimit() && this->channel[j].getUserCount() >= (size_t)this->channel[j].getUserLimit())
    {
        notifyClient2(471, getClientByFd(fd)->getNickname(), "#" + param[i].first, getClientByFd(fd)->getFduser(), " :Cannot join channel (+l)\r\n");
        return;
    }
    User *user = getClientByFd(fd);
    this->channel[j].addUser(*user);
    if (channel[j].getTopicName().empty())
        notifyUsers(RPL_JOIN(getClientByFd(fd)->getHostname(), getClientByFd(fd)->getIp(), param[i].first) +
                    RPL_NAMREPLY(getClientByFd(fd)->getNickname(), channel[j].getChannelName(), channel[j].getUserList()) +
                    RPL_ENDOFNAMES(getClientByFd(fd)->getNickname(), channel[j].getChannelName()), fd);
    else
        notifyUsers(RPL_JOIN(getClientByFd(fd)->getHostname(), getClientByFd(fd)->getIp(), param[i].first) +
                    RPL_TOPIC(getClientByFd(fd)->getNickname(), channel[j].getChannelName(), channel[j].getTopicName()) +
                    RPL_NAMREPLY(getClientByFd(fd)->getNickname(), channel[j].getChannelName(), channel[j].getUserList()) +
                    RPL_ENDOFNAMES(getClientByFd(fd)->getNickname(), channel[j].getChannelName()), fd);
    channel[j].broadcastMessage2(RPL_JOIN(getClientByFd(fd)->getHostname(), getClientByFd(fd)->getIp(), param[i].first), fd);
}

// Ajouter un utilisateur dans un nouveau canal qu'on va créer
void Server::createAndAddToNewChannel(std::vector<std::pair<std::string, std::string> > &param, int i, int fd)
{
    User *user = getClientByFd(fd);
    Channel newChannel;

    if (channelUserCount(getClientByFd(fd)->getNickname()) >= 20)
    {
        notifyClient3(405, getClientByFd(fd)->getNickname(), getClientByFd(fd)->getFduser(), " :You have joined too many channels\r\n");
        return;
    }
    newChannel.setChannelName(param[i].first);
    user->setChannelFounder(true);
    newChannel.addOperatorOnChannel(*user);
    if (!user->isOperator())
        user->setOperator(true);
    newChannel.setCreationDate();
    this->channel.push_back(newChannel);
    notifyUsers(RPL_JOIN(getClientByFd(fd)->getHostname(), getClientByFd(fd)->getIp(), newChannel.getChannelName()) +
                RPL_NAMREPLY(getClientByFd(fd)->getNickname(), newChannel.getChannelName(), newChannel.getUserList()) +
                RPL_ENDOFNAMES(getClientByFd(fd)->getNickname(), newChannel.getChannelName()), fd);
}

// Compter le nombre de canaux dont l'utilisateur est membre
int Server::channelUserCount(std::string user)
{
    int count = 0;

    for (size_t i = 0; i < this->channel.size(); i++)
    {
        if (this->channel[i].getFindUserByName(user))
            count++;
    }
    return count;
}

// Vérifie si l'utilisateur a été invité à rejoindre un canal
bool Server::isUserInvited(User *user, std::string channel, int flag)
{
    if (user->isInvited(channel))
    {
        if (flag == 1)
            user->removeInvitation(channel);
        return true;
    }
    return false;
}
