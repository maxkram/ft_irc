#include "../../headers/Command.hpp"

void Command::quit(std::vector<std::string> cmds, Client &client)
{
	if (client.isRegistered())
	{
		if (cmds.size() > 1 && client.isConnected())
		{
			if (cmds[1][0] == ':')
				cmds[1].erase(0, 1);
			sendConfirm(client, cmds[0], cmds[1]);
		}
		else if (client.isConnected())
			sendConfirm(client, cmds[0], client.getNick());
		std::vector<std::string>::iterator it = client.getChannels().begin();
		std::map<std::string, Channel>::iterator	itMap;
		for (; it != client.getChannels().end(); it++)
		{
			itMap = chanMap.find(*it);
			itMap->second.deleteClient(&client);
			if(itMap->second.isChannelEmpty())
				chanMap.erase(itMap);
		}
		std::list<Client>::iterator itClient = clients.begin();
		for (; itClient != clients.end(); itClient++)
		{
			if (&(*itClient) != &client)
			{
				for (it = client.getChannels().begin(); it != client.getChannels().end(); it++)
				{
					if (itClient->isInChannel(*it))
					{
						if (cmds.size() > 1)
						{
							if (cmds[1][0] == ':')
								cmds[1].erase(0, 1);
							sendConfirmTo(*itClient, client, cmds[0], cmds[1]);
						}
						else
							sendConfirmTo(*itClient, client, cmds[0], client.getNick());
						break;
					}
				}
			}
		}
	}
	if (client.isConnected())
		closeLink(client, "Closing Link", client.getIP() + " (Client Quit)");
	close(client.getSocket());
	client.setMarkedForDeletion(true);
}
