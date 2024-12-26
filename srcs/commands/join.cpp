#include "../../includes/server.hpp"

void Server::JOIN(std::string message, int fd)
{
    std::vector<std::pair<std::string, std::string> > param;
    User *user = getClientByFd(fd);
    
    if (!splitJoinParams(param, message, fd))
    {
        notifyClient3(461, user->getNickname(), user->getFduser(), " :Not enough parameters\r\n");
        return;
    }
    if (param.size() > 20)
    {
        notifyClient3(407, user->getNickname(), user->getFduser(), " :Too many channels\r\n");
        return;
    }
    for (size_t i = 0; i < param.size(); ++i)
    {
        bool channelExists = false;
        for (size_t j = 0; j < this->channel.size(); ++j)
        {
            if (this->channel[j].getChannelName() == param[i].first)
            {
                addClientToExistChannel(param, i, j, fd);
                channelExists = true;
                break;
            }
        }
        if (!channelExists)
            createAndAddToNewChannel(param, i, fd);
    }
}

int Server::splitJoinParams(std::vector<std::pair<std::string, std::string> > &param, std::string message, int fd)
{
    std::vector<std::string> vec;
    std::istringstream ss(message);
    std::string token, channel, key;

    while (std::getline(ss, token, ' '))
        vec.push_back(token);

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
    std::string tempChannel;
    for (size_t i = 0; i < channel.size(); ++i)
    {
        if (channel[i] == ',')
        {
            param.push_back(std::make_pair(tempChannel, ""));
            tempChannel.clear();
        }
        else
            tempChannel += channel[i];
    }
    param.push_back(std::make_pair(tempChannel, ""));

    if (!key.empty())
    {
        size_t j = 0;
        std::string tempKey;
        for (size_t i = 0; i < key.size(); ++i)
        {
            if (key[i] == ',')
            {
                param[j].second = tempKey;
                j++;
                tempKey.clear();
            }
            else
                tempKey += key[i];
        }
        param[j].second = tempKey;
    }
    for (size_t i = 0; i < param.size(); ++i)
    {
        if (param[i].first.empty())
            param.erase(param.begin() + i--);
    }
    for (size_t i = 0; i < param.size(); ++i)
    {
        if (*(param[i].first.begin()) != '#')
        {
            notifyClient2(403, getClientByFd(fd)->getNickname(), param[i].first, getClientByFd(fd)->getFduser(), " :No such channel\r\n");
            param.erase(param.begin() + i--);
        }
        else
            param[i].first.erase(0, 1);
    }
    return 1;
}

void Server::addClientToExistChannel(std::vector<std::pair<std::string, std::string> > &param, int i, int j, int fd)
{
    User *user = getClientByFd(fd);
    if (!user) {
        std::cerr << "Error: Invalid user for existing channel." << std::endl;
        return;
    }

    if (this->channel[j].getFindUserByName(user->getNickname()))
        return;
    if (channelUserCount(user->getNickname()) >= 20) {
        notifyClient3(405, user->getNickname(), user->getFduser(), " :You have joined too many channels\r\n");
        return;
    }
    if (!this->channel[j].getChannelPassword().empty() &&
        this->channel[j].getChannelPassword() != param[i].second &&
        !isUserInvited(user, param[i].first, 0)) {        
            notifyClient2(475, user->getNickname(), "#" + param[i].first, user->getFduser(), " :Cannot join channel (+k) - bad key\r\n");
            return;
    }
    if (this->channel[j].getInviteOnlyStatus() && !isUserInvited(getClientByFd(fd), param[i].first, 1)) {
        notifyClient2(473, user->getNickname(), "#" + param[i].first, user->getFduser(), " :Cannot join channel (+i)\r\n");
        return;
    }
    if (this->channel[j].getUserLimit() && this->channel[j].getUserCount() >= (size_t)this->channel[j].getUserLimit()) {
        notifyClient2(471, user->getNickname(), "#" + param[i].first, user->getFduser(), " :Cannot join channel (+l)\r\n");
        return;
    }

    this->channel[j].addUser(*user);

    if (channel[j].getTopicName().empty())
        notifyUsers(RPL_JOIN(user->getHostname(), user->getIp(), param[i].first) +
                    RPL_NAMREPLY(user->getNickname(), channel[j].getChannelName(), channel[j].getUserList()) +
                    RPL_ENDOFNAMES(getClientByFd(fd)->getNickname(), channel[j].getChannelName()), fd);
    else
        notifyUsers(RPL_JOIN(user->getHostname(), user->getIp(), param[i].first) +
                    RPL_TOPIC(user->getNickname(), channel[j].getChannelName(), channel[j].getTopicName()) +
                    RPL_NAMREPLY(user->getNickname(), channel[j].getChannelName(), channel[j].getUserList()) +
                    RPL_ENDOFNAMES(user->getNickname(), channel[j].getChannelName()), fd);
    channel[j].broadcastMessage2(RPL_JOIN(user->getHostname(), user->getIp(), param[i].first), fd);
}

void Server::createAndAddToNewChannel(std::vector<std::pair<std::string, std::string> > &param, int i, int fd)
{
    User *user = getClientByFd(fd);
    if (!user) {
        std::cerr << "Error: Invalid user trying to create a new channel." << std::endl;
        return;
    }
    
    if (channelUserCount(user->getNickname()) >= 20) {
        notifyClient3(405, user->getNickname(), user->getFduser(), " :You have joined too many channels\r\n");
        return;
    }

    Channel newChannel;
    newChannel.setChannelName(param[i].first);
    user->setChannelFounder(true);
    newChannel.addOperatorOnChannel(*user);
    if (!user->isOperator())
        user->setOperator(true);
    newChannel.setCreationDate();
    this->channel.push_back(newChannel);
    notifyUsers(RPL_JOIN(user->getHostname(), user->getIp(), newChannel.getChannelName()) +
                RPL_NAMREPLY(user->getNickname(), newChannel.getChannelName(), newChannel.getUserList()) +
                RPL_ENDOFNAMES(user->getNickname(), newChannel.getChannelName()), fd);
}

int Server::channelUserCount(std::string user)
{
    int count = 0;

    for (size_t i = 0; i < this->channel.size(); i++)
    {
        if (this->channel[i].getFindUserByName(user))
            ++count;
    }
    return count;
}

bool Server::isUserInvited(User *user, std::string channel, int flag)
{
    // Check if the user is invited to the specified channel
    if (!user->isInvited(channel)) {
        return false;
    }

    // If flag is 1, remove the invitation after confirmation
    if (flag == 1) {
        user->removeInvitation(channel);
    }

    return true;
}
