#include "../../headers/Command.hpp"

void	Command::ping(std::vector<std::string> cmds, Client & client)
{
    // Check if enough parameters are provided
    if (cmds.size() == 1)
    {
        sendMessage(client, "461", cmds[0], ERR_NEEDMOREPARAMS);
        return;
    }
    // Send the PONG response
    sendSpecConfirm(client, IP, "PONG", cmds[1]);
}
