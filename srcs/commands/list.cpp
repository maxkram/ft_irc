#include "../../headers/Command.hpp"

void Command::list(std::vector<std::string> cmds, Client & client)
{
    if (!client.isRegistered())
    {
        sendMessage(client, "451", "", ERR_NOTREGISTERED);
		return ;
	}
    std::map<std::string, Channel>::iterator itMap;
    std::list<Client>::iterator	itClient;
    sendMessage(client, "321", "", "Channel :Users  Name");
    if (cmds.size() == 1)
    {
        itMap = chanMap.begin();
        std::string message;
        for (; itMap != chanMap.end(); itMap++)
        {
            std::string channel(itMap->first);
            itClient = clients.begin();
            if (!message.empty())
                message = "";
            for(; itClient != clients.end(); itClient++)
            {
                if (itClient->isInChannel(channel) == true)
                    message.append(itMap->second.getTopic());
            }
            if (message[0] == ':')
                message.erase(0,1);
            sendMessage(client, "322", channel + " :", message);
        }
    }
    else if (cmds.size() > 1)
    {
        std::vector<std::string>	keys;
        std::vector<std::string>::iterator	iChan;
        std::string message;
        keys = splitString(cmds[1], ",");
        iChan = keys.begin();
        for (; iChan != keys.end(); iChan++)
        {
            if (!message.empty())
                message = "";
            itMap = chanMap.begin();
            for (; itMap != chanMap.end(); itMap++)
            {
                if (itMap->first == *iChan)
                {
                    itClient = clients.begin();
                    for (; itClient != clients.end(); itClient++)
                    {
                        if (itClient->isInChannel(itMap->first) == true)
                            message.append(itMap->second.getTopic());
                    }
                    if (message[0] == ':')
                        message.erase(0,1);
                    sendMessage(client, "322", itMap->first + " :", message);
                }
            }
        }
    }
    if (cmds.size() == 1)
        sendMessage(client, "323", "", ":End of /NAMES list.");
    else    
        sendMessage(client, "323", cmds[1] +  " :End of", "/NAMES list.");
    return;
}
