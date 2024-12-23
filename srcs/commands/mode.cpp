// #include "../../includes/server.hpp"

// void Server::MODE_CHANNEL(std::string &message, int fd)
// {
//     User *user;
//     Channel *chan;
//     char addminus;
//     size_t nonspace;
//     std::string channelname;
//     std::string modestring;
//     std::stringstream ssmode;
//     std::string param;
//     std::string arg;
//     std::vector<std::string> paramsplit;

//     addminus = '\0';
//     arg.clear();
//     modestring.clear();
//     user = getClientByFd(fd);
//     nonspace = message.find_first_not_of("MODEmode \t\v");
//     if (nonspace != std::string::npos)
//         message = message.substr(nonspace);
//     else
//     {
//         notifyUsers(ERR_NOTENOUGHPARAMETERS(user->getNickname()), fd);
//         return;
//     }
//     parseChannelMode(message, channelname, modestring, param);
//     paramsplit = splitChannelMode(param);
//     chan = getChannel(channelname.substr(1));

//     if (channelname[0] != '#' || !chan)
//     {
//         notifyUsers(ERR_NOSUCHCHANNEL(user->getUser(), channelname), fd);
//         return;
//     }
//     else if (!chan->getUserByFd(fd) && !chan->getOperatorByFd(fd))
//     {
//         notifyClient2(442, getClientByFd(fd)->getNickname(), channelname, getClientByFd(fd)->getFduser(), " :You're not on that channel\r\n");
//         return;
//     }
//     else if (modestring.empty())
//     {
//         notifyUsers(RPL_CHANNELMODE(user->getNickname(), chan->getChannelName(), chan->getChannelModes()) + \
//                     RPL_CREATIONTIME(user->getNickname(), chan->getChannelName(), chan->getCreationDate()), fd);
//         return;
//     }
//     else if (!chan->getOperatorByFd(fd))
//     {
//         notifyUsers(ERR_NOTOPERATOR(user->getNickname(), chan->getChannelName()), fd);
//         return;
//     }
//     else if (chan)
//     {
//         size_t pos = 0;
//         for (size_t i = 0; i < modestring.size(); i++)
//         {
//             if (modestring[i] == '+' || modestring[i] == '-')
//                 addminus = modestring[i];
//             else
//             {
//                 if (modestring[i] == 'i')
//                     ssmode << inviteOnly(chan, addminus, ssmode.str());
//                 else if (modestring[i] == 't')
//                     ssmode << topicRestriction(chan, addminus, ssmode.str());
//                 else if (modestring[i] == 'k')
//                     ssmode << channelPassword(chan, addminus, ssmode.str(), paramsplit, arg, pos, fd);
//                 else if (modestring[i] == 'o')
//                     ssmode << operatorPrivilege(chan, addminus, ssmode.str(), paramsplit, arg, pos, fd);
//                 else if (modestring[i] == 'l')
//                     ssmode << setUserLimit(chan, addminus, ssmode.str(), paramsplit, arg, pos, fd);
//                 else
//                     notifyUsers(ERR_UNKNOWNMODE(user->getNickname(), chan->getChannelName(), modestring[i]), fd);
//             }
//         }
//     }

//     std::string chain = ssmode.str();
//     if (chain.empty())
//         return;
//     chan->broadcastMessage(RPL_CHANGEMODE(user->getHostname(), chan->getChannelName(), ssmode.str(), arg));
// }

// void Server::parseChannelMode(std::string message, std::string &channelname, std::string &modestring, std::string &param)
// {
//     std::istringstream ss(message);
//     size_t nonspace;

//     ss >> channelname;
//     ss >> modestring;
//     nonspace = message.find_first_not_of(channelname + modestring + " \t\v");
//     if (nonspace != std::string::npos)
//         param = message.substr(nonspace);
// }

// std::vector<std::string> Server::splitChannelMode(std::string param)
// {
//     std::vector<std::string> paramsplit;
//     std::string line;
//     std::istringstream ss(param);

//     if (!param.empty() && param[0] == ':')
//         param.erase(param.begin());
//     while (std::getline(ss, line, ','))
//     {
//         paramsplit.push_back(line);
//         line.clear();
//     }
//     return paramsplit;
// }

// std::string Server::updateMode(std::string ssmode, char addminus, char mode)
// {
//     std::stringstream ss;

//     ss.clear();
//     char nul = '\0';
//     for (size_t i = 0; i < ssmode.size(); i++)
//     {
//         if (ssmode[i] == '+' || ssmode[i] == '-')
//             nul = ssmode[i];
//     }
//     if (nul != addminus)
//         ss << addminus << mode;
//     else
//         ss << mode;
//     return ss.str();
// }

// std::string Server::inviteOnly(Channel *channel, char addminus, std::string ssmode)
// {
//     std::string str;

//     str.clear();
//     if (addminus == '+' && !channel->getChannelModeOption(0))
//     {
//         channel->setChannelMode(0, true);
//         channel->setInviteOnlyStatus(1);
//         str = updateMode(ssmode, addminus, 'i');
//     }
//     else if (addminus == '-' && channel->getChannelModeOption(0))
//     {
//         channel->setChannelMode(0, false);
//         channel->setInviteOnlyStatus(0);
//         str = updateMode(ssmode, addminus, 'i');
//     }
//     return str;
// }

// std::string Server::topicRestriction(Channel *channel, char addminus, std::string ssmode)
// {
//     std::string str;

//     str.clear();
//     if (addminus == '+' && !channel->getChannelModeOption(1))
//     {
//         channel->setChannelMode(1, true);
//         channel->setTopicRestriction(true);
//         str = updateMode(ssmode, addminus, 't');
//     }
//     else if (addminus == '-' && channel->getChannelModeOption(1))
//     {
//         channel->setChannelMode(1, false);
//         channel->setTopicRestriction(false);
//         str = updateMode(ssmode, addminus, 't');
//     }
//     return str;
// }

// std::string Server::channelPassword(Channel *channel, char addminus, std::string ssmode, std::vector<std::string> paramsplit, std::string &arg, size_t &pos, int fd)
// {
//     std::string str;
//     std::string password;

//     str.clear();
//     password.clear();
//     if (paramsplit.size() > pos)
//     {
//         password = paramsplit[pos++];
//     }
//     else
//     {
//         notifyUsers(ERR_NEEDMODEPARAM(channel->getChannelName(), std::string("(k)")), fd);
//         return str;
//     }
//     if (!isValidChannelPassword(password))
//     {
//         notifyUsers(ERR_NEEDMODEPARAM(channel->getChannelName(), std::string("(k)")), fd);
//         return str;
//     }
//     if (addminus == '+')
//     {
//         channel->setChannelMode(2, true);
//         channel->setChannelPassword(password);
//         if (!arg.empty())
//             arg += " ";
//         arg += password;
//         str = updateMode(ssmode, addminus, 'k');
//     }
//     else if (addminus == '-' && channel->getChannelModeOption(2))
//     {
//         if (password == channel->getChannelPassword())
//         {
//             channel->setChannelMode(2, false);
//             channel->setChannelPassword("");
//             str = updateMode(ssmode, addminus, 'k');
//         }
//         else
//             notifyUsers(ERR_KEYEXIST(channel->getChannelName()), fd);
//     }
//     return str;
// }

// bool Server::isValidChannelPassword(std::string password)
// {
//     if (password.empty())
//         return false;
//     for (size_t i = 0; i < password.size(); i++)
//     {
//         if (!std::isalnum(password[i]) && password[i] != '_')
//             return false;
//     }
//     return true;
// }


// std::string Server::operatorPrivilege(Channel *channel, char addminus, std::string ssmode, std::vector<std::string> paramsplit, std::string &arg, size_t &pos, int fd)
// {
//     std::string str;
//     std::string user;

//     str.clear();
//     user.clear();
//     if (paramsplit.size() > pos)
//         user = paramsplit[pos++];
//     else
//     {
//         notifyUsers(ERR_NEEDMODEPARAM(channel->getChannelName(), std::string("(o)")), fd);
//         return str;
//     }
//     if (!channel->isUserInChannel(user))
//     {
//         notifyUsers(ERR_NOSUCHNICKCHAN(channel->getChannelName(), user), fd);
//         return str;
//     }
//     if (addminus == '+')
//     {
//         channel->setChannelMode(3, true);
//         if (channel->promoteUserToOperator(user))
//         {
//             str = updateMode(ssmode, addminus, 'o');
//             if (!arg.empty())
//                 arg += " ";
//             arg += user;
//         }
//     }
//     else if (addminus == '-')
//     {
//         channel->setChannelMode(3, false);
//         if (channel->demoteOperatorToUser(user))
//         {
//             str = updateMode(ssmode, addminus, 'o');
//             if (!arg.empty())
//                 arg += " ";
//             arg += user;
//         }
//     }
//     return str;
// }

// bool Server::isValidUserLimit(std::string &limit)
// {
//     return (!(limit.find_first_not_of("0123456789") != std::string::npos) && std::atoi(limit.c_str()) > 0);
// }

// std::string Server::setUserLimit(Channel *channel, char addminus, std::string ssmode, std::vector<std::string> paramsplit, std::string &arg, size_t &pos, int fd)
// {
//     std::string str;
//     std::string limit;

//     str.clear();
//     limit.clear();
//     if (addminus == '+')
//     {
//         if (paramsplit.size() > pos)
//         {
//             limit += paramsplit[pos++];
//             if (isValidUserLimit(limit))
//             {
//                 channel->setChannelMode(4, true);
//                 channel->setUserLimit(std::atoi(limit.c_str()));
//                 if (!arg.empty())
//                     arg += " ";
//                 arg += limit;
//                 str = updateMode(ssmode, addminus, 'l');
//             }
//             else
//                 notifyUsers(ERR_INVALIDMODEPARAM(channel->getChannelName(), std::string("(l)")), fd);
//         }
//         else
//             notifyUsers(ERR_NEEDMODEPARAM(channel->getChannelName(), std::string("(l)")), fd);
//     }
//     else if (addminus == '-' && channel->getChannelModeOption(4))
//     {
//         channel->setChannelMode(4, false);
//         channel->setUserLimit(0);
//         str = updateMode(ssmode, addminus, 'l');
//     }
//     return str;
// }



#include "../../includes/server.hpp"

void Server::MODE_CHANNEL(std::string &message, int fd)
{
    User *user = getClientByFd(fd);
    if (!user)
        return;
    Channel *chan = NULL;
    std::string channelname, modestring, param, arg;
    std::stringstream ssmode;
    std::vector<std::string> paramsplit;

    char addminus = '\0';
    arg.clear();
    modestring.clear();
    size_t nonspace = message.find_first_not_of("MODEmode \t\v");
    if (nonspace != std::string::npos)
        message = message.substr(nonspace);
    else
    {
        notifyUsers(ERR_NOTENOUGHPARAMETERS(user->getNickname()), fd);
        return;
    }
    parseChannelMode(message, channelname, modestring, param);
    paramsplit = splitChannelMode(param);
    chan = getChannel(channelname.substr(1));

    if (channelname[0] != '#' || !chan)
    {
        notifyUsers(ERR_NOSUCHCHANNEL(user->getUser(), channelname), fd);
        return;
    }
    if (!chan->getUserByFd(fd) && !chan->getOperatorByFd(fd))
    {
        notifyClient2(442, getClientByFd(fd)->getNickname(), channelname, getClientByFd(fd)->getFduser(), " :You're not on that channel\r\n");
        return;
    }
    if (modestring.empty())
    {
        notifyUsers(RPL_CHANNELMODE(user->getNickname(), chan->getChannelName(), chan->getChannelModes()) + \
                    RPL_CREATIONTIME(user->getNickname(), chan->getChannelName(), chan->getCreationDate()), fd);
        return;
    }
    else if (!chan->getOperatorByFd(fd))
    {
        notifyUsers(ERR_NOTOPERATOR(user->getNickname(), chan->getChannelName()), fd);
        return;
    }
    // Process mode changes
    size_t pos = 0;
    for (size_t i = 0; i < modestring.size(); i++)
    {
        if (modestring[i] == '+' || modestring[i] == '-')
        {
            addminus = modestring[i];
        }
        else
        {
            switch (modestring[i])
            {
                case 'i':
                    ssmode << inviteOnly(chan, addminus, ssmode.str());
                    break;
                case 't':
                    ssmode << topicRestriction(chan, addminus, ssmode.str());
                    break;
                case 'k':
                    ssmode << channelPassword(chan, addminus, ssmode.str(), paramsplit, arg, pos, fd);
                    break;
                case 'o':
                    ssmode << operatorPrivilege(chan, addminus, ssmode.str(), paramsplit, arg, pos, fd);
                    break;
                case 'l':
                    ssmode << setUserLimit(chan, addminus, ssmode.str(), paramsplit, arg, pos, fd);
                    break;
                default:
                    notifyUsers(ERR_UNKNOWNMODE(user->getNickname(), chan->getChannelName(), modestring[i]), fd);
            }
        }
    }

    std::string chain = ssmode.str();
    if (chain.empty())
        return;
    chan->broadcastMessage(RPL_CHANGEMODE(user->getHostname(), chan->getChannelName(), ssmode.str(), arg));
}

void Server::parseChannelMode(std::string message, std::string &channelname, std::string &modestring, std::string &param)
{
    std::istringstream ss(message);

    ss >> channelname;
    ss >> modestring;

    size_t nonspace = message.find_first_not_of(channelname + modestring + " \t\v");
    if (nonspace != std::string::npos)
        param = message.substr(nonspace);
}

std::vector<std::string> Server::splitChannelMode(std::string param)
{
    std::vector<std::string> paramsplit;
    std::string line;
    std::istringstream ss(param);

    if (!param.empty() && param[0] == ':')
        param.erase(param.begin());
    while (std::getline(ss, line, ','))
        paramsplit.push_back(line);

    return paramsplit;
}

std::string Server::updateMode(std::string ssmode, char addminus, char mode)
{
    if (ssmode.empty() || ssmode.find(addminus) == std::string::npos)
        return std::string(1, addminus) + mode;

    return ssmode + mode;
}

std::string Server::inviteOnly(Channel *channel, char addminus, std::string ssmode)
{
    if (addminus == '+' && !channel->getChannelModeOption(0))
    {
        channel->setChannelMode(0, true);
        channel->setInviteOnlyStatus(1);
        return updateMode(ssmode, addminus, 'i');
    }
    else if (addminus == '-' && channel->getChannelModeOption(0))
    {
        channel->setChannelMode(0, false);
        channel->setInviteOnlyStatus(0);
        return updateMode(ssmode, addminus, 'i');
    }
    return ssmode;
}

std::string Server::topicRestriction(Channel *channel, char addminus, std::string ssmode)
{
    if (addminus == '+' && !channel->getChannelModeOption(1))
    {
        channel->setChannelMode(1, true);
        channel->setTopicRestriction(true);
        return updateMode(ssmode, addminus, 't');
    }
    else if (addminus == '-' && channel->getChannelModeOption(1))
    {
        channel->setChannelMode(1, false);
        channel->setTopicRestriction(false);
        return updateMode(ssmode, addminus, 't');
    }
    return ssmode;
}

std::string Server::channelPassword(Channel *channel, char addminus, std::string ssmode, std::vector<std::string> paramsplit, std::string &arg, size_t &pos, int fd)
{
    std::string str;
    std::string password;

    str.clear();
    password.clear();
    if (paramsplit.size() > pos)
    {
        password = paramsplit[pos++];
    }
    else
    {
        notifyUsers(ERR_NEEDMODEPARAM(channel->getChannelName(), std::string("(k)")), fd);
        return str;
    }
    if (!isValidChannelPassword(password))
    {
        notifyUsers(ERR_NEEDMODEPARAM(channel->getChannelName(), std::string("(k)")), fd);
        return str;
    }
    if (addminus == '+')
    {
        channel->setChannelMode(2, true);
        channel->setChannelPassword(password);
        if (!arg.empty())
            arg += " ";
        arg += password;
        str = updateMode(ssmode, addminus, 'k');
    }
    else if (addminus == '-' && channel->getChannelModeOption(2))
    {
        if (password == channel->getChannelPassword())
        {
            channel->setChannelMode(2, false);
            channel->setChannelPassword("");
            str = updateMode(ssmode, addminus, 'k');
        }
        else
            notifyUsers(ERR_KEYEXIST(channel->getChannelName()), fd);
    }
    return str;
}

bool Server::isValidChannelPassword(std::string password)
{
    if (password.empty())
        return false;
    for (size_t i = 0; i < password.size(); i++)
    {
        if (!std::isalnum(password[i]) && password[i] != '_')
            return false;
    }
    return true;
}


std::string Server::operatorPrivilege(Channel *channel, char addminus, std::string ssmode, std::vector<std::string> paramsplit, std::string &arg, size_t &pos, int fd)
{
    std::string str;
    std::string user;

    str.clear();
    user.clear();
    if (paramsplit.size() > pos)
        user = paramsplit[pos++];
    else
    {
        notifyUsers(ERR_NEEDMODEPARAM(channel->getChannelName(), std::string("(o)")), fd);
        return str;
    }
    if (!channel->isUserInChannel(user))
    {
        notifyUsers(ERR_NOSUCHNICKCHAN(channel->getChannelName(), user), fd);
        return str;
    }
    if (addminus == '+')
    {
        channel->setChannelMode(3, true);
        if (channel->promoteUserToOperator(user))
        {
            str = updateMode(ssmode, addminus, 'o');
            if (!arg.empty())
                arg += " ";
            arg += user;
        }
    }
    else if (addminus == '-')
    {
        channel->setChannelMode(3, false);
        if (channel->demoteOperatorToUser(user))
        {
            str = updateMode(ssmode, addminus, 'o');
            if (!arg.empty())
                arg += " ";
            arg += user;
        }
    }
    return str;
}

bool Server::isValidUserLimit(std::string &limit)
{
    return (!(limit.find_first_not_of("0123456789") != std::string::npos) && std::atoi(limit.c_str()) > 0);
}

std::string Server::setUserLimit(Channel *channel, char addminus, std::string ssmode, std::vector<std::string> paramsplit, std::string &arg, size_t &pos, int fd)
{
    std::string str;
    std::string limit;

    str.clear();
    limit.clear();
    if (addminus == '+')
    {
        if (paramsplit.size() > pos)
        {
            limit += paramsplit[pos++];
            if (isValidUserLimit(limit))
            {
                channel->setChannelMode(4, true);
                channel->setUserLimit(std::atoi(limit.c_str()));
                if (!arg.empty())
                    arg += " ";
                arg += limit;
                str = updateMode(ssmode, addminus, 'l');
            }
            else
                notifyUsers(ERR_INVALIDMODEPARAM(channel->getChannelName(), std::string("(l)")), fd);
        }
        else
            notifyUsers(ERR_NEEDMODEPARAM(channel->getChannelName(), std::string("(l)")), fd);
    }
    else if (addminus == '-' && channel->getChannelModeOption(4))
    {
        channel->setChannelMode(4, false);
        channel->setUserLimit(0);
        str = updateMode(ssmode, addminus, 'l');
    }
    return str;
}