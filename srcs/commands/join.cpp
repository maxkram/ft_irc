#include "../../headers/Command.hpp"

void	Command::join(std::vector<std::string> cmds, Client & client)
{
	// Step 1: Client registration check
	if (!client.isRegistered())
	{
		sendMessage(client, "451", "", ERR_NOTREGISTERED);
		return ;
	}
	// Step 2: Check for sufficient parameters
	if (cmds.size() < 2)
	{
		sendMessage(client, "461", cmds[0], ERR_NEEDMOREPARAMS);
		return ;
	}
	// Step 3: Handle "JOIN 0" (leave all channels)
	if (cmds[1] == "0")
	{
		std::vector<std::string>::iterator	it = client.getChannels().begin();
		std::vector<std::string>	partCmd;
		partCmd.push_back("PART ");
		partCmd.push_back("");
		for (;it != client.getChannels().end(); it++)
			partCmd[1].append(*it + ",");
		part(partCmd, client);
		return;
	}
	// Step 4: Split channels and keys
	std::vector<std::string>	channels = splitString(cmds[1], ",");
	std::vector<std::string>	keys;
	std::vector<std::string>	names;
	names.push_back("names");
	names.push_back(cmds[1]);
	if (cmds.size() > 2)
		keys = splitString(cmds[2], ",");
	std::vector<std::string>::iterator	it = channels.begin();
	std::vector<std::string>::iterator	itKey = keys.begin();
	std::map<std::string, Channel>::iterator	itMap;
	for(; it != channels.end(); it++)
	{
		// Step 5: Channel name validation
		if (it->length() > 50 || (*it)[0] != '#')
			sendMessage(client, "403", cmds[0], *it + ERR_NOSUCHCHANNEL);
		// Step 6: Create channel if it doesn't exist
		else if ((itMap = chanMap.find(*it)) == chanMap.end())
		{
			itMap =	chanMap.insert(std::pair<std::string, Channel>(*it, Channel())).first;
			// Set channel operator and optional password
			itMap->second.addChannelOperator(&client);
			if (itKey != keys.end())
				itMap->second.setPassword(*itKey);
			client.getChannels().push_back(*it);
			sendConfirm(client, cmds[0] + " " + *it, "");
			sendMessage(client, "324", cmds[1], getUsedModes(cmds[1]));
			Command::names(names, client);
			itMap->second.sendChannelMessage(client, cmds[0] + " " + *it, "");
		}
		else
		{
			// Step 7: Verify access to existing channel
			if (!client.isInChannel(*it))
			{
				if (!itMap->second.getPassword().empty() && (itKey == keys.end() || itMap->second.getPassword() != *itKey))
				{
					sendMessage(client, "475", *it + ERR_BADCHANNELKEY, "");
				}
				else if (itMap->second.isInviteEnabled() && !itMap->second.isClientInvited(client))
				{
					sendMessage(client, "473", *it + ERR_INVITEONLYCHAN, "");
				}
				else
				{
					// Add client to the channel
					itMap->second.addClient(&client);
					client.getChannels().push_back(*it);
					sendConfirm(client, cmds[0] + " " + *it, "");
					sendMessage(client, "324", cmds[1], getUsedModes(cmds[1]));
					Command::names(names, client);
					// Send channel topic if it exists
					if (!itMap->second.getTopic().empty())
					{
						sendMessage(client, "332", *it, itMap->second.getTopic());
						sendMessage(client, "333", *it, itMap->second.getTopicAuthor());
					}
					itMap->second.sendChannelMessage(client, cmds[0] + " " + *it, "");
				}
			}
		}
		if (itKey != keys.end())
			itKey++;
	}
}
