#include "../../headers/Command.hpp"

void Command::whois(std::vector<std::string> cmds, Client & client)
{
    if (!client.isRegistered())
    {
        sendMessage(client, "451", "", ERR_NOTREGISTERED);
        return;
    }
    if (cmds.size() == 1)
    {
        sendMessage(client, "431", "", ERR_NONICKNAMEGIVEN);
        return;
    }
    std::list<Client>::iterator	it = clients.begin();
	std::map<std::string, Channel>::iterator	itMap = chanMap.begin();
    std::time_t result = std::time(nullptr);
	for(; it != clients.end(); it++)
		if (cmds[1] == it->getNick())
			break;
    if (it == clients.end())
    {
        sendMessage(client, "401", cmds[1], ERR_NOSUCHNICK);
        return;
    }
    sendMessage(client, "311", it->getNick() + " ~" + it->getUsername() + " " + it->getIP() + " *" , ":" + it->getRealName());
    sendMessage(client, "312", it->getNick() + " " + IP , ":Amsterdam, NL");
    if (it->getAwayStatus().first)
    {
        if (it->getAwayStatus().second[0] == ':')
            sendMessage(client, "301", it->getNick(), it->getAwayStatus().second);
        else
            sendMessage(client, "301", it->getNick(), ":" + it->getAwayStatus().second);
    }
    std::string operChan;
    for(; itMap != chanMap.end(); itMap++)
    {
        if (itMap->second.isChannelOperator(*it))
            operChan.append("@" + itMap->first + " ");         
    }
    if (operChan.size() > 0)
        sendMessage(client, "319",it->getNick(), ":" + operChan);
    sendMessage(client, "338", it->getNick() + " " + it->getIP(), ":actually using host");
    sendMessage(client, "317", it->getNick() + " " + std::to_string((int)result), ":seconds idle, signon time");
    sendMessage(client, "318", it->getNick(), RPL_ENDOFWHOIS);
    return;
}
