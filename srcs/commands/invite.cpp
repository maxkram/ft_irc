#include "../../headers/Command.hpp"

void    Command::invite(std::vector<std::string> cmds, Client & client)
{
	// Step 1: Check if the client is registered
	if (!client.isRegistered())
	{
		sendMessage(client, "451", "", ERR_NOTREGISTERED);
		return ;
	}
	// Step 2: Validate the number of arguments
	if (cmds.size() < 2)
	{
		sendMessage(client, "461", cmds[0], ERR_NEEDMOREPARAMS);
		return ;
	}
	// Step 3: Find the target client by nickname
	std::list<Client>::iterator	it = clients.begin();
	for (; it != clients.end(); it++)
		if (it->getNick() == cmds[1])
			break;
	if (it == clients.end())
	{
        sendMessage(client, "401", cmds[1], ERR_NOSUCHNICK);
        return;
	}
	// Step 4: Check if the inviter is in the specified channel
	if (!client.isInChannel(cmds[2]))
	{
		sendMessage(client, "442", cmds[2], ERR_NOTONCHANNEL);
		return;
	}
	// Step 5: Check if the inviter is a channel operator
	if (!chanMap[cmds[2]].isChannelOperator(client))
	{
		sendMessage(client, "482", cmds[2] + " ", ERR_CHANOPRIVSNEEDED);
		return;
	}
	// Step 6: Check if the target client is already in the channel
	if (it->isInChannel(cmds[2]))
	{
		sendMessage(client, "443", cmds[1] + " " + cmds[2], ERR_USERONCHANNEL);
		return;
	}
	// Step 7: Send an invitation and confirmation
	sendMessage(client, "341", cmds[1] + " " + cmds[2], "");
	sendConfirmTo(*it, client, cmds[0] + " " + it->getNick(), cmds[2]);
	std::map<std::string, Channel>::iterator	itMap;
	if ((itMap = chanMap.find(cmds[2])) != chanMap.end())
	{
		itMap->second.addInvitedClient(&(*it));
		std::cout << itMap->first << std::endl;	
	}
	else
	{
		sendMessage(client, "401", cmds[1], ERR_NOSUCHNICK);
		return;
	}
	// Step 8: Notify if the target user is away
	if (it->getAwayStatus().first)
	{
		sendMessage(client, "301", it->getNick(), it->getAwayStatus().second);
		return;
	}
}
