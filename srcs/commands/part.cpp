#include "../../headers/Command.hpp"

void Command::part(std::vector<std::string> cmds, Client &client)
{
	if (!client.isRegistered())
	{
		sendMessage(client, "451", "", ERR_NOTREGISTERED);
		return;
	}
	if (cmds.size() < 2)
	{
		sendMessage(client, "461", cmds[0], ERR_NEEDMOREPARAMS);
		return;
	}
	std::vector<std::string> chans = splitString(cmds[1], ",");
	std::vector<std::string>::iterator it = chans.begin();
	std::map<std::string, Channel>::iterator itMap;
	for (; it != chans.end(); it++)
	{
		if ((itMap = chanMap.find(*it)) == chanMap.end())
			sendMessage(client, "403", cmds[0], *it + ERR_NOSUCHCHANNEL);
		else
		{
			if (client.isInChannel(*it))
			{
				sendConfirm(client, cmds[0] + " " + *it, "");
				itMap->second.sendChannelMessage(client, cmds[0] + " " + *it, "");
				itMap->second.deleteClient(&client);
				client.removeChannel(*it);
				if (itMap->second.isChannelEmpty())
					chanMap.erase(itMap);
			}
			else
				sendMessage(client, "442", cmds[0], *it + ERR_NOTONCHANNEL);
		}
	}
}
