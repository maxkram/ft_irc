#include "../../includes/server.hpp"

void Server::JOIN(std::string message, int fd)
{
    std::vector<std::pair<std::string, std::string> > param;
    User *client = getClientByFd(fd);
    // Split the JOIN parameters
    if (!splitJoinParameters(param, message, fd))
    {
        sendError(461, client->getNickname(), client->getFduser(), " :Not enough parameters\r\n");
        return;
    }

    // Check if the user is attempting to join too many channels
    if (param.size() > 20)
    {
        sendError(407, client->getNickname(), client->getFduser(), " :Too many channels\r\n");
        return;
    }

    // Process each channel in the parameters
    for (size_t i = 0; i < param.size(); ++i)
    {
        bool found = false;

        // Check if the channel already exists
        for (size_t j = 0; j < channel.size(); ++j)
        {
            if (channel[j].getChannelName() == param[i].first)
            {
                addClientToExistChannel(param, i, j, fd);
                found = true;
                break;
            }
        }

        // If the channel does not exist, create a new one
        if (!found)
        {
            createAndAddToNewChannel(param, i, fd);
        }
    }
}

int Server::splitJoinParameters(std::vector<std::pair<std::string, std::string> > &param, std::string message, int fd)
{
    std::vector<std::string> vec;
    std::istringstream ss(message);
    std::string line, channel, key, str;

    if (message.empty()) {
        param.clear();
        return 0; // Early exit for invalid input
    }

    // Split the input message by spaces
    while (std::getline(ss, line, ' '))
        vec.push_back(line);

    if (vec.size() < 2)
    {
        param.clear();
        return 0;
    }

    // Extract channels and keys
    vec.erase(vec.begin());
    channel = vec[0];
    vec.erase(vec.begin());
    if (!vec.empty())
        key = vec[0];

    // Split channels by ',' and add to param
    for (size_t i = 0; i < channel.size(); ++i)
    {
        if (channel[i] == ',')
        {
            param.push_back(std::pair<std::string, std::string>(str, ""));
            str.clear();
        }
        else
        {
            str += channel[i];
        }
    }
    param.push_back(std::pair<std::string, std::string>(str, ""));

    // Split keys by ',' and associate with channels
    if (!key.empty())
    {
        size_t j = 0;
        str.clear();
        for (size_t i = 0; i < key.size(); ++i)
        {
            if (key[i] == ',')
            {
                param[j++].second = str;
                str.clear();
            }
            else
            {
                str += key[i];
            }
        }
        param[j].second = str;
    }

    // Remove invalid or empty channels
    for (size_t i = 0; i < param.size(); ++i)
    {
        if (param[i].first.empty())
        {
            param.erase(param.begin() + i--);
            continue;
        }
        if (param[i].first[0] != '#')
        {
            sendErrorToClient(403, getClientByFd(fd)->getNickname(), param[i].first, getClientByFd(fd)->getFduser(), " :No such channel\r\n");
            param.erase(param.begin() + i--);
        }
        else
        {
            param[i].first.erase(param[i].first.begin());
        }
    }

    return 1;
}

void Server::addClientToExistChannel(std::vector<std::pair<std::string, std::string> > &param, int i, int j, int fd)
{
    User *user = getClientByFd(fd);

    // Check if the user is already in the channel
    if (channel[j].getFindUserByName(user->getNickname()))
        return;

    // Check if the user has joined too many channels
    if (channelUserCount(user->getNickname()) >= 20)
    {
        sendError(405, user->getNickname(), user->getFduser(), " :You have joined too many channels\r\n");
        return;
    }

    // Validate channel password
    if (!channel[j].getChannelPassword().empty() && channel[j].getChannelPassword() != param[i].second)
    {
        if (!isUserInvited(user, param[i].first, 0))
        {
            sendErrorToClient(475, user->getNickname(), "#" + param[i].first, user->getFduser(), " :Cannot join channel (+k) - bad key\r\n");
            return;
        }
    }

    // Check if the channel is invite-only
    if (channel[j].isInviteOnly())
    {
        if (!isUserInvited(user, param[i].first, 1))
        {
            sendErrorToClient(473, user->getNickname(), "#" + param[i].first, user->getFduser(), " :Cannot join channel (+i)\r\n");
            return;
        }
    }

    // Check if the channel has reached its user limit
    if (channel[j].getUserLimit() && channel[j].getUserCount() >= (size_t)channel[j].getUserLimit())
    {
        sendErrorToClient(471, user->getNickname(), "#" + param[i].first, user->getFduser(), " :Cannot join channel (+l)\r\n");
        return;
    }

    // Add the user to the channel
    channel[j].addRegularUser(*user);

    // Notify the user and others in the channel
    std::string joinMessage = RPL_CHANNEL_JOIN(user->getHostname(), user->getIp(), param[i].first);
    std::string nameReply = RPL_CHANNEL_NAME_REPLY(user->getNickname(), channel[j].getChannelName(), channel[j].getUserList());
    std::string endOfNames = RPL_CHANNEL_NAME_END(user->getNickname(), channel[j].getChannelName());

    if (channel[j].getTopicName().empty())
    {
        notifyUsers(joinMessage + nameReply + endOfNames, fd);
    }
    else
    {
        std::string topicReply = RPL_TOPIC(user->getNickname(), channel[j].getChannelName(), channel[j].getTopicName());
        notifyUsers(joinMessage + topicReply + nameReply + endOfNames, fd);
    }

    channel[j].sendMessageToAllExcept(joinMessage, fd);
}


void Server::createAndAddToNewChannel(std::vector<std::pair<std::string, std::string> > &param, int i, int fd)
{
    User *user = getClientByFd(fd);
    Channel newChannel;

    // Check if the user has joined too many channels
    if (channelUserCount(user->getNickname()) >= 20)
    {
        sendError(405, user->getNickname(), user->getFduser(), " :You have joined too many channels\r\n");
        return;
    }

    // Initialize the new channel
    newChannel.setChannelName(param[i].first);
    user->setChannelFounder(true);
    newChannel.addOperator(*user);

    if (!user->isOperator())
        user->setOperator(true);

    newChannel.setCreationDate();
    channel.push_back(newChannel);

    // Notify the user about the new channel creation and membership
    notifyUsers(
        RPL_CHANNEL_JOIN(user->getHostname(), user->getIp(), newChannel.getChannelName()) +
        RPL_CHANNEL_NAME_REPLY(user->getNickname(), newChannel.getChannelName(), newChannel.getUserList()) +
        RPL_CHANNEL_NAME_END(user->getNickname(), newChannel.getChannelName()),
        fd
    );
}

int Server::channelUserCount(std::string user)
{
    int count = 0;

    for (size_t i = 0; i < channel.size(); i++)
    {
        if (channel[i].getFindUserByName(user))
            count++;
    }
    return count;
}

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