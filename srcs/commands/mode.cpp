#include "../../includes/server.hpp"

void Server::MODE_CHANNEL(std::string &message, int fd)
{
    User *user = getClientByFd(fd);
    if (!user)
        return;

    Channel *chan = NULL;
    char addminus = '\0';
    std::string channelname, modestring, param, arg;
    std::stringstream ssmode;
    std::vector<std::string> paramsplit;

    // Remove leading spaces and the command prefix
    size_t nonspace = message.find_first_not_of("MODEmode \t\v");
    if (nonspace != std::string::npos)
        message = message.substr(nonspace);
    else
    {
        notifyUsers(ERR_MISSING_PARAMETERS(user->getNickname()), fd);
        return;
    }

    // Parse the message for channel mode details
    parseChannelMode(message, channelname, modestring, param);
    paramsplit = splitChannelMode(param);

    // Validate the channel
    chan = getChannel(channelname.substr(1));
    if (channelname[0] != '#' || !chan)
    {
        notifyUsers(ERR_CHANNEL_NOT_FOUND(user->getUser(), channelname), fd);
        return;
    }
    else if (!chan->getUserByFd(fd) && !chan->getOperatorByFd(fd))
    {
        sendErrorToClient(442, user->getNickname(), channelname, user->getFduser(), " :You're not on that channel\r\n");
        return;
    }
    else if (modestring.empty())
    {
        notifyUsers(RPL_CHANNELMODE(user->getNickname(), chan->getChannelName(), chan->getChannelModes()) +
                    RPL_CREATIONTIME(user->getNickname(), chan->getChannelName(), chan->getCreationDate()), fd);
        return;
    }
    else if (!chan->getOperatorByFd(fd))
    {
        notifyUsers(ERR_CHANNEL_NOT_OPERATOR(user->getNickname(), chan->getChannelName()), fd);
        return;
    }

    // Process the mode changes
    size_t pos = 0;
    for (size_t i = 0; i < modestring.size(); ++i)
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
                    notifyUsers(ERR_UNKNOWN_MODE(user->getNickname(), chan->getChannelName(), modestring[i]), fd);
                    break;
            }
        }
    }

    // Broadcast the mode change
    std::string chain = ssmode.str();
    if (!chain.empty())
    {
        chan->broadcastMessage(RPL_CHANGEMODE(user->getHostname(), chan->getChannelName(), chain, arg));
    }
}

void Server::parseChannelMode(std::string message, std::string &channelname, std::string &modestring, std::string &param)
{
    std::istringstream ss(message);
    size_t nonspace;

    // Extract the channel name and mode string
    ss >> channelname;
    ss >> modestring;

    // Find the first non-space character after the extracted strings
    nonspace = message.find_first_not_of(channelname + modestring + " \t\v");
    if (nonspace != std::string::npos)
    {
        param = message.substr(nonspace);
    }
}


std::vector<std::string> Server::splitChannelMode(std::string param)
{
    std::vector<std::string> paramsplit;
    std::string line;
    std::istringstream ss(param);

    // Remove leading ':' if present
    if (!param.empty() && param[0] == ':')
        param.erase(param.begin());

    // Split the string by ',' and populate the vector
    while (std::getline(ss, line, ','))
    {
        paramsplit.push_back(line);
    }

    return paramsplit;
}

std::string Server::applyModeChange(std::string ssmode, char addminus, char mode)
{
    std::stringstream ss;
    char currentMode = '\0';

    // Find the last '+' or '-' in the current mode string
    for (size_t i = 0; i < ssmode.size(); ++i)
    {
        if (ssmode[i] == '+' || ssmode[i] == '-')
            currentMode = ssmode[i];
    }

    // Append the mode change based on the current and new mode
    if (currentMode != addminus)
        ss << addminus;
    ss << mode;

    return ss.str();
}

std::string Server::inviteOnly(Channel *channel, char addminus, std::string ssmode)
{
    std::string str;

    if (addminus == '+' && !channel->isModeEnabled(0))
    {
        channel->setChannelMode(0, true);
        channel->setInviteOnly(1);
        str = applyModeChange(ssmode, addminus, 'i');
    }
    else if (addminus == '-' && channel->isModeEnabled(0))
    {
        channel->setChannelMode(0, false);
        channel->setInviteOnly(0);
        str = applyModeChange(ssmode, addminus, 'i');
    }

    return str;
}

std::string Server::topicRestriction(Channel *channel, char addminus, std::string ssmode)
{
    std::string str;

    str.clear();
    if (addminus == '+' && !channel->isModeEnabled(1))
    {
        channel->setChannelMode(1, true);
        channel->setTopicRestriction(true);
        str = applyModeChange(ssmode, addminus, 't');
    }
    else if (addminus == '-' && channel->isModeEnabled(1))
    {
        channel->setChannelMode(1, false);
        channel->setTopicRestriction(false);
        str = applyModeChange(ssmode, addminus, 't');
    }
    return str;
}

std::string Server::channelPassword(Channel *channel, char addminus, std::string ssmode, std::vector<std::string> paramsplit, std::string &arg, size_t &pos, int fd)
{
    std::string str;
    std::string password;

    // Check if a password parameter is provided
    if (paramsplit.size() > pos)
    {
        password = paramsplit[pos++];
    }
    else
    {
        notifyUsers(ERR_NEEDMODEPARAM(channel->getChannelName(), "(k)"), fd);
        return str;
    }

    // Validate the password format
    if (!validateChannelPassword(password))
    {
        notifyUsers(ERR_NEEDMODEPARAM(channel->getChannelName(), "(k)"), fd);
        return str;
    }

    // Handle adding or removing the password mode
    if (addminus == '+')
    {
        channel->setChannelMode(2, true);
        channel->setPassword(password);

        if (!arg.empty())
            arg += " ";
        arg += password;

        str = applyModeChange(ssmode, addminus, 'k');
    }
    else if (addminus == '-' && channel->isModeEnabled(2))
    {
        if (password == channel->getChannelPassword())
        {
            channel->setChannelMode(2, false);
            channel->setPassword("");
            str = applyModeChange(ssmode, addminus, 'k');
        }
        else
        {
            notifyUsers(ERR_KEYEXIST(channel->getChannelName()), fd);
        }
    }

    return str;
}

bool Server::validateChannelPassword(std::string password)
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

    // Check if a user parameter is provided
    if (paramsplit.size() > pos)
    {
        user = paramsplit[pos++];
    }
    else
    {
        notifyUsers(ERR_NEEDMODEPARAM(channel->getChannelName(), "(o)"), fd);
        return str;
    }

    // Validate if the user is in the channel
    if (!channel->isUserInChannel(user))
    {
        notifyUsers(ERR_NO_SUCH_NICK_CHANNEL(channel->getChannelName(), user), fd);
        return str;
    }

    // Handle promotion or demotion based on the mode
    if (addminus == '+')
    {
        channel->setChannelMode(3, true);
        if (channel->promoteToOperator(user))
        {
            str = applyModeChange(ssmode, addminus, 'o');
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
            str = applyModeChange(ssmode, addminus, 'o');
            if (!arg.empty())
                arg += " ";
            arg += user;
        }
    }

    return str;
}

bool Server::validateUserLimit(std::string &limit)
{
    // Ensure the limit contains only digits and is greater than zero
    return (limit.find_first_not_of("0123456789") == std::string::npos && std::atoi(limit.c_str()) > 0);
}

std::string Server::setUserLimit(Channel *channel, char addminus, std::string ssmode, std::vector<std::string> paramsplit, std::string &arg, size_t &pos, int fd)
{
    std::string str;
    std::string limit;

    if (addminus == '+')
    {
        if (paramsplit.size() > pos)
        {
            limit = paramsplit[pos++];
            if (validateUserLimit(limit))
            {
                channel->setChannelMode(4, true);
                channel->setUserLimit(std::atoi(limit.c_str()));

                if (!arg.empty())
                    arg += " ";
                arg += limit;

                str = applyModeChange(ssmode, addminus, 'l');
            }
            else
            {
                notifyUsers(ERR_INVALIDMODEPARAM(channel->getChannelName(), "(l)"), fd);
            }
        }
        else
        {
            notifyUsers(ERR_NEEDMODEPARAM(channel->getChannelName(), "(l)"), fd);
        }
    }
    else if (addminus == '-' && channel->isModeEnabled(4))
    {
        channel->setChannelMode(4, false);
        channel->setUserLimit(0);

        str = applyModeChange(ssmode, addminus, 'l');
    }

    return str;
}
