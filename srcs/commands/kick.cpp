#include "../../headers/Command.hpp"

void Command::kick(std::vector<std::string> cmds, Client & client)
{
	if (!client.isRegistered())
    {
        sendMessage(client, "451", "", ERR_NOTREGISTERED);
        return;
    }
	if (cmds.size() < 3)
	{
		sendMessage(client, "461", cmds[0] ,ERR_NEEDMOREPARAMS);
		return;
	}
	std::map<std::string, Channel>::iterator	itMap;
	if ((itMap = chanMap.find(cmds[1])) == chanMap.end())
	{
		sendMessage(client, "403", cmds[0], cmds[1] + ERR_NOSUCHCHANNEL);
		return ;
	}
	if (!client.isInChannel(cmds[1]))
	{
		sendMessage(client, "442", cmds[1], ERR_NOTONCHANNEL);
		return ;
	}
	if (!itMap->second.isChannelOperator(client))
	{
		sendMessage(client, "482", cmds[1] , ERR_CHANOPRIVSNEEDED);
		return ;
	}
	std::list<Client>::iterator	it = clients.begin();
	for(; it != clients.end(); it++)
		if (cmds[2] == it->getNick())
			break;
	if (it == clients.end() || !it->isInChannel(cmds[1]))
	{
		sendMessage(client, "441", cmds[2] + " " + cmds[1], ERR_USERNOTINCHANNEL);
		return ;
	}
	if (cmds.size() == 4)
	{
		if (cmds[3][0] == ':')
			cmds[3].erase(0, 1);
		sendConfirm(*it, cmds[0] + " " + cmds[1] + " " + it->getNick(), cmds[3]);
		itMap->second.sendChannelMessage(*it, cmds[0] + " " + cmds[1] + " " + it->getNick(), cmds[3]);
	}
	else
	{
		sendConfirm(*it, cmds[0] + " " + cmds[1] ,it->getNick());
		itMap->second.sendChannelMessage(*it, cmds[0] + " " + cmds[1] , it->getNick());
	}
	itMap->second.deleteClient(&(*it));
	it->removeChannel(cmds[1]);
	if (itMap->second.isChannelEmpty())
		chanMap.erase(itMap);
}
