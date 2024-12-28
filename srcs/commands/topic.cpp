#include "../../includes/server.hpp"

void Server::TOPIC(std::string message, int fd)
{
	std::vector<std::string> param = extractParams(message);
	User *user = getClientByFd(fd);
	
	if (!user)
    {
        std::cerr << "Error: User not found for fd " << fd << std::endl;
        return;
    }

	std::string split_message[3] = {std::string(), std::string(), std::string()};
	
	if (splitMessage(message, split_message))
		return;
	
	std::string split_params[3] = {std::string(), std::string(), std::string()};
	
	if (splitParams(split_message[2], split_params) == 1)
		return;
	
	if (param.size() < 2)
	{
		notifyUsers(ERR_NOTENOUGHPARAMETERS(user->getUser()), fd);
		return;
	}
	
	std::string channame = param[1].substr(1);
	
	Channel *channelPtr = getChannel(channame);

	if (!channelPtr)
	{
		sendError(403, "#" + channame, fd, " :No such channel\r\n");
		return;
	}
	
	if (!channelPtr->getUserByFd(fd) && !channelPtr->getOperatorByFd(fd))
	{
		sendError(442, "#" + channame, fd, " :You're not on that channel\r\n");
		return;
	}
	
	Channel ch = Channel();

	for (size_t i = 0; i < channel.size(); i++)
	{
		if (channel[i].getChannelName() == &(split_params[0][1]))
			ch = channel[i];
	}
	
	// Check topic restriction and user privileges
    if (channelPtr->isTopicRestricted() && !channelPtr->isUserOperator(user->getNickname()))
    {
        notifyUsers(ERR_NOTOPERATOR(user->getNickname(), channame), fd);
        return;
    }
	
    // Handle topic retrieval or update
    std::string topic;
    if (param.size() == 2) // No topic provided, retrieve current topic
    {
        if (channelPtr->getTopicName().empty())
        {
            notifyUsers(RPL_NOTOPICSET(channelPtr->getChannelName()), fd);
        }
        else
        {
            channelPtr->broadcastMessage(RPL_TOPIC(user->getNickname(), channelPtr->getChannelName(), channelPtr->getTopicName()));
            channelPtr->broadcastMessage(RPL_TOPICWHOTIME(user->getNickname(), channelPtr->getChannelName(), user->getNickname(), channelPtr->getCreationDate()));
        }
    }
    else // Update the topic
    {
        topic = param[2].substr(1); // Remove leading ':' from the topic
        channelPtr->setTopic(topic);
        channelPtr->broadcastMessage(RPL_TOPIC(user->getNickname(), channelPtr->getChannelName(), channelPtr->getTopicName()));
        channelPtr->broadcastMessage(RPL_TOPICWHOTIME(user->getNickname(), channelPtr->getChannelName(), user->getNickname(), channelPtr->getCreationDate()));
    }
}