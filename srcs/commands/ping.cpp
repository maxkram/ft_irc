#include "../../headers/Command.hpp"

void	Command::ping(std::vector<std::string> cmds, Client & client)
{
    if (cmds.size() == 1)
    {
        sendMessage(client, "461", cmds[0], ERR_NEEDMOREPARAMS);
        return;
    }
    sendSpecConfirm(client, IP, "PONG", cmds[1]);
    return;
}
