#include "../../headers/Command.hpp"

void	Command::who(std::vector<std::string> cmds, Client & client)
{
    if(!client.isRegistered())
    {
        sendMessage(client, "451", "", ERR_NOTREGISTERED);
        return;
    }
    if (cmds.size() == 1)
    {
        sendMessage(client, "461", cmds[0], ERR_NEEDMOREPARAMS);
        return;
    } 
    std::list<Client>::iterator	it = clients.begin();
	std::map<std::string, Channel>::iterator	itMap = chanMap.begin();
    std::string message;
    std::string channel;
    if (cmds[1][0] == '#')
    {
        for (; itMap != chanMap.end(); itMap++)
        {
            if (itMap->first == cmds[1])
            {
                for(; it != clients.end(); it++)
                {
                    if (!message.empty())
                        message = "";
                    if (itMap->second.isChannelOperator(*it) == true)
                        message.append("H@ :0 " + it->getRealName() + " ");
                    else if (it->isInChannel(itMap->first) == true)
                        message.append("H :0 " + it->getRealName() + " ");
                    sendMessage(client, "352", cmds[1] + " ~" + it->getUsername() + " " + it->getIP() + " " + IP + " " + it->getNick() , message);
                }
            }
        }
        sendMessage(client, "315", cmds[1], RPL_ENDOFWHO);
    }
    else
    {
        for(; it != clients.end(); it++)
        {
            if (it->getNick() == cmds[1])
            {
                for (; itMap != chanMap.end(); itMap++)
                {
                    if (it->isInChannel(itMap->first))
                    {
                        if (itMap->second.isChannelOperator(*it) == true)
                        {
                            channel.append(itMap->first);
                            message.append("H@ :0 " + it->getRealName() + " ");
                            break;
                        }
                        else
                        {
                            channel.append(itMap->first);
                            message.append("H :0 " + it->getRealName() + " ");
                            break;
                        }
                    }
                }
                if (itMap == chanMap.end())
                {
                    channel.append("* ");
                    message.append("H :0" + it->getRealName() + " ");
                }
                sendMessage(client, "352", channel + " ~" + it->getUsername() + " " + it->getIP() + " " + IP + " " + it->getNick() , message);
            }
        }
        sendMessage(client, "315", cmds[1], RPL_ENDOFWHO);
    }
    return;
}
