#include "../../headers/Command.hpp"

void	Command::away(std::vector<std::string> cmds, Client & client)
{
    if (!client.isRegistered())
    {
        sendMessage(client, "451", "", ERR_NOTREGISTERED);
        return;
    }
    if (cmds.size() == 1)
    {
        client.setAwayStatus(std::pair<bool, std::string>(false, ""));
        sendMessage(client, "305", "", RPL_UNAWAY);
        return;
    }
    if (cmds.size() > 1)
    {
        client.setAwayStatus(std::pair<bool, std::string>(true, cmds[1]));
        sendMessage(client, "306", "", RPL_NOWAWAY);
        return;
    }
}
