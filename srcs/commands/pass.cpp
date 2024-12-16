#include "../../headers/Command.hpp"

void	Command::pass(std::vector<std::string> cmds, Client & client)
{
	if (cmds.size() == 1)
	{
		sendMessage(client, "461", "", ERR_NEEDMOREPARAMS);
		return ;
	}
	if (client.isRegistered())
	{
		sendMessage(client, "462", "", ERR_ALREADYREGISTRED);
		return;
	}
	client.setPass(cmds[1]);
}
