// #include "../../includes/server.hpp"

// // Cette fonction gère la commande TOPIC, qui permet de définir ou d'afficher le sujet d'un canal
// void Server::TOPIC(std::string message, int fd)
// {
// 	std::vector<std::string> param;
// 	User *user = getClientByFd(fd);
// 	std::string channame;

// 	param = extractParams(message);
// 	std::string split_message[3] = {std::string(), std::string(), std::string()};
// 	if (splitMessage(message, split_message))
// 		return;
	
// 	std::string split_params[3] = {std::string(), std::string(), std::string()};
// 	if (splitParams(split_message[2], split_params) == 1)
// 		return;
// 	if (param.size() < 2)
// 	{
// 		notifyUsers(ERR_NOTENOUGHPARAMETERS(getClientByFd(fd)->getUser()), fd);
// 		return;
// 	}
// 	channame = param[1].substr(1);
// 	if (!getChannel(channame))
// 	{
// 		notifyClient3(403, "#" + channame, fd, " :No such channel\r\n");
// 		return;
// 	}
// 	if (!(getChannel(channame)->getUserByFd(fd)) && !(getChannel(channame)->getOperatorByFd(fd)))
// 	{
// 		notifyClient3(442, "#" + channame, fd, " :You're not on that channel\r\n");
// 		return;
// 	}
// 	Channel ch = Channel();

// 	for (size_t i = 0; i < channel.size(); i++)
// 	{
// 		if (channel[i].getChannelName() == &(split_params[0][1]))
// 			ch = channel[i];
// 	}
// 	if (ch.getTopicRestriction() && !ch.isUserOperator(user->getNickname()))
// 	{
// 		notifyUsers(ERR_NOTOPERATOR(user->getNickname(), channame), fd);
// 		return;
// 	}
// 	if (!split_params[0].empty() && split_params[1].empty())
// 	{
// 		if (ch.getTopicName().empty() == 1)
// 			notifyUsers(RPL_NOTOPICSET(ch.getChannelName()), fd);
// 		else
// 		{
// 			ch.broadcastMessage(RPL_TOPIC(getClientByFd(fd)->getNickname(), ch.getChannelName(), ch.getTopicName()));
// 			ch.broadcastMessage(RPL_TOPICWHOTIME(getClientByFd(fd)->getNickname(), ch.getChannelName(), getClientByFd(fd)->getNickname(), ch.getCreationDate()));
// 		}
// 		return;
// 	}
// 	else if (ch.getUserByFd(fd) != NULL || ch.getOperatorByFd(fd) != NULL)
// 	{
// 		ch.setTopicName(split_params[1].substr(1));
// 		ch.broadcastMessage(RPL_TOPIC(getClientByFd(fd)->getNickname(), ch.getChannelName(), ch.getTopicName()));
// 		ch.broadcastMessage(RPL_TOPICWHOTIME(getClientByFd(fd)->getNickname(), ch.getChannelName(), getClientByFd(fd)->getNickname(), ch.getCreationDate()));
// 		return;
// 	}
// }


#include "../../includes/server.hpp"

// Handles the TOPIC command to set or display a channel's topic
void Server::TOPIC(std::string message, int fd)
{
	// Get the user issuing the command
	
	User *user = getClientByFd(fd);
    // if (!user) {
    //     notifyUsers(ERR_NOTREGISTERED("*"), fd);
    //     return;
    // }
	
	// Extract parameters from the message
	std::vector<std::string> param = extractParams(message);

	std::string split_message[3] = {std::string(), std::string(), std::string()};
	if (splitMessage(message, split_message))
		return;
	
	std::string split_params[3] = {std::string(), std::string(), std::string()};
	if (splitParams(split_message[2], split_params) == 1)
		return;

	// Check if enough parameters are provided
	if (param.size() < 2) {
		notifyUsers(ERR_NOTENOUGHPARAMETERS(getClientByFd(fd)->getUser()), fd);
		return;
	}
	// Extract the channel name and validate it
	std::string channame = param[1].substr(1);
	if (!getChannel(channame)) {
		notifyClient3(403, "#" + channame, fd, " :No such channel\r\n");
		return;
	}
	if (!(getChannel(channame)->getUserByFd(fd)) && !(getChannel(channame)->getOperatorByFd(fd))) {
		notifyClient3(442, "#" + channame, fd, " :You're not on that channel\r\n");
		return;
	}
	Channel ch = Channel();

	for (size_t i = 0; i < channel.size(); i++) {
		if (channel[i].getChannelName() == &(split_params[0][1]))
			ch = channel[i];
	}
	// Check for topic restriction and operator status
	if (ch.getTopicRestriction() && !ch.isUserOperator(user->getNickname())) {
		notifyUsers(ERR_NOTOPERATOR(user->getNickname(), channame), fd);
		return;
	}
	// Determine if the command is for displaying or setting the topic
	if (!split_params[0].empty() && split_params[1].empty()) {
		if (ch.getTopicName().empty())
			notifyUsers(RPL_NOTOPICSET(ch.getChannelName()), fd);
		else {
			ch.broadcastMessage(RPL_TOPIC(getClientByFd(fd)->getNickname(), ch.getChannelName(), ch.getTopicName()));
			ch.broadcastMessage(RPL_TOPICWHOTIME(getClientByFd(fd)->getNickname(), ch.getChannelName(), getClientByFd(fd)->getNickname(), ch.getCreationDate()));
		}
		return;
	} // Set topic
	else if (ch.getUserByFd(fd) != NULL || ch.getOperatorByFd(fd) != NULL) {
		ch.setTopicName(split_params[1].substr(1));
		ch.broadcastMessage(RPL_TOPIC(getClientByFd(fd)->getNickname(), ch.getChannelName(), ch.getTopicName()));
		ch.broadcastMessage(RPL_TOPICWHOTIME(getClientByFd(fd)->getNickname(), ch.getChannelName(), getClientByFd(fd)->getNickname(), ch.getCreationDate()));
		return;
	}
}


