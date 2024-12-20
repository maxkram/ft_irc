#include "../../includes/server.hpp"

// Fonction pour gérer la commande PART (permettre à un utilisateur de quitter un canal)
void Server::PART(std::string message, int fd)
{
    std::vector<std::string> param;
    std::string reason;
    bool flag;

    if (!splitPartParams(message, param, reason, fd))
    {
        notifyClient3(461, getClientByFd(fd)->getNickname(), getClientByFd(fd)->getFduser(), " :Not enough parameters\r\n");
        return;
    }
    for (size_t i = 0; i < param.size(); i++)
    {
        flag = false;
        for (size_t j = 0; j < this->channel.size(); j++)
        {
            if (this->channel[j].getChannelName() == param[i])
            {
                flag = true;
                if (!channel[j].getUserByFd(fd) && !channel[j].getOperatorByFd(fd))
                {
                    notifyClient2(442, getClientByFd(fd)->getNickname(), "#" + param[i], getClientByFd(fd)->getFduser(), " :You're not on that channel\r\n");
                    continue;
                }
                if (channel[j].getFindUserByName(getClientByFd(fd)->getNickname())->isChannelFounder())
                    channel[j].getFindUserByName(getClientByFd(fd)->getNickname())->setChannelFounder(false);
                std::stringstream ss;
                ss << ":" << getClientByFd(fd)->getNickname() << "!~" << getClientByFd(fd)->getUser() << "@localhost PART #" << param[i];
                if (!reason.empty())
                    ss << " :" << reason << "\r\n";
                else
                    ss << "\r\n";
                channel[j].broadcastMessage(ss.str());
                if (channel[j].getOperatorByFd(channel[j].getFindUserByName(getClientByFd(fd)->getNickname())->getFduser()))
                    channel[j].removeOperatorByFd(channel[j].getFindUserByName(getClientByFd(fd)->getNickname())->getFduser());
                else
                    channel[j].removeUserByFd(channel[j].getFindUserByName(getClientByFd(fd)->getNickname())->getFduser());
                if (channel[j].getUserCount() == 0)
                    channel.erase(channel.begin() + j);
                else
                {
                    if (!channel[j].hasOperators())
                    {
                        channel[j].promoteFirstUserToOperator();
                    }
                }
            }
        }
        if (!flag)
            notifyClient2(403, getClientByFd(fd)->getNickname(), "#" + param[i], getClientByFd(fd)->getFduser(), " :No such channel\r\n");
    }
}

// Fonction pour séparer la commande PART en paramètres individuels et extraire la raison de départ
int Server::splitPartParams(std::string message, std::vector<std::string> &param, std::string &reason, int fd)
{
    std::string str;
    std::string tmp;

    reason = extractPartReason(message, param);
    if (param.size() < 2)
    {
        param.clear();
        return 0;
    }
    param.erase(param.begin());
    str = param[0];
    param.clear();
    for (size_t i = 0; i < str.size(); i++)
    {
        if (str[i] == ',')
        {
            param.push_back(tmp);
            tmp.clear();
        }
        else
            tmp += str[i];
    }
    param.push_back(tmp);
    for (size_t i = 0; i < param.size(); i++)
    {
        if (param[i].empty())
            param.erase(param.begin() + i--);
    }
    if (reason[0] == ':')
        reason.erase(reason.begin());
    else
    {
        for (size_t i = 0; i < reason.size(); i++)
        {
            if (reason[i] == ' ')
            {
                reason = reason.substr(0, i);
                break;
            }
        }
    }
    for (size_t i = 0; i < param.size(); i++)
    {
        if (*(param[i].begin()) == '#')
            param[i].erase(param[i].begin());
        else
        {
            notifyClient2(403, getClientByFd(fd)->getNickname(), param[i], getClientByFd(fd)->getFduser(), " :No such channel\r\n");
            param.erase(param.begin() + i--);
        }
    }
    return 1;
}

// Fonction pour diviser les paramètres en deux (channel et reason)
// et les stocker dans le vecteur param. Ensuite, extraire et retourner la raison.
std::string Server::extractPartReason(std::string &message, std::vector<std::string> &param)
{
    int count;
    std::stringstream ss(message);
    std::string str; 
    std::string reason;

    count = 2;
    while (ss >> str && count--)
        param.push_back(str);
    if (param.size() != 2)
        return std::string("");
    partReason(message, param[1], reason);

    return reason;
}

// Fonction pour trouver la première occurrence de tofind dans message
// et stocker tout ce qui se trouve après cette occurrence dans reason.
void Server::partReason(std::string message, std::string tofind, std::string &reason)
{
    size_t i;
    std::string str;

    i = 0;
    for (; i < message.size(); i++)
    {
        if (message[i] != ' ')
        {
            for (; i < message.size() && message[i] != ' '; i++)
                str += message[i];
            if (str == tofind)
                break;
            else
                str.clear();
        }
    }
    if (i < message.size())
        reason = message.substr(i);
    i = 0;
    for (; i < reason.size() && reason[i] == ' '; i++);
    reason = reason.substr(i);
}
