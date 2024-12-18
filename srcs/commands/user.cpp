#include "../../headers/Command.hpp"

void	Command::user(std::vector<std::string> cmds, Client & client)
{
	// Check if the client is already registered
	if (client.isRegistered())
	{
		sendMessage(client, "462", "", ERR_ALREADYREGISTRED);
		return;
	}
	// Check if there are enough parameters
	if (cmds.size() <= 4 || cmds[4] == ":")
	{
		sendMessage(client, "461", cmds[0], ERR_NEEDMOREPARAMS);
		return;
	}
	// Set client attributes
	client.setUsername(cmds[1]);
	client.setMode(cmds[2]);
	client.setUnused(cmds[3]);
	// Process real name (removing leading ':' if present)
	if (cmds[4][0] == ':')
		cmds[4].erase(0, 1);
	client.setRealName(cmds[4]);
	// Finalize registration
	client.setUsered(true);
	registerClient(client);
}
