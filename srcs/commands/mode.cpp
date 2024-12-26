#include "../../includes/server.hpp"

void Server::MODE_CHANNEL(std::string &message, int fd)
{
    User *user = getClientByFd(fd);
    if (!user) { return; }
     
    std::string channelname, modeString, param, arg;

    std::stringstream ssmode;

    char addminus = '\0';
    arg.clear();
    modeString.clear();
    size_t nonspace = message.find_first_not_of("MODEmode \t\v");
    
    if (nonspace == std::string::npos) {
        notifyUsers(ERR_NOTENOUGHPARAMETERS(user->getNickname()), fd);
        return;
    }

    message = message.substr(nonspace);

    parseChannelMode(message, channelname, modeString, param);

    // Validate channel name
    if (channelname.empty() || channelname[0] != '#') {
        notifyUsers(ERR_NOSUCHCHANNEL(user->getUser(), channelname), fd);
        return;
    }

    std::vector<std::string> paramsplit = splitChannelMode(param);
    Channel *chan = getChannel(channelname.substr(1));

    if (channelname[0] != '#' || !chan)
    {
        notifyUsers(ERR_NOSUCHCHANNEL(user->getUser(), channelname), fd);
        return;
    }
    // Permission checks
    if (!chan->getUserByFd(fd) && !chan->getOperatorByFd(fd))
    {
        notifyClient2(442, getClientByFd(fd)->getNickname(), channelname, getClientByFd(fd)->getFduser(), " :You're not on that channel\r\n");
        return;
    }
    if (modeString.empty())
    {
        notifyUsers(RPL_CHANNELMODE(user->getNickname(), chan->getChannelName(), chan->getChannelModes()) + \
                    RPL_CREATIONTIME(user->getNickname(), chan->getChannelName(), chan->getCreationDate()), fd);
        return;
    }
    if (!chan->getOperatorByFd(fd))
    {
        notifyUsers(ERR_NOTOPERATOR(user->getNickname(), chan->getChannelName()), fd);
        return;
    }

    size_t pos = 0;
    for (size_t i = 0; i < modeString.size(); ++i)
    {
        if (modeString[i] == '+' || modeString[i] == '-')
            addminus = modeString[i];
        else
        {
            if (modeString[i] == 'i')
                ssmode << inviteOnly(chan, addminus, ssmode.str());
            else if (modeString[i] == 't')
                ssmode << topicRestriction(chan, addminus, ssmode.str());
            else if (modeString[i] == 'k')
                ssmode << channelPassword(chan, addminus, ssmode.str(), paramsplit, arg, pos, fd);
            else if (modeString[i] == 'o')
                ssmode << operatorPrivilege(chan, addminus, ssmode.str(), paramsplit, arg, pos, fd);
            else if (modeString[i] == 'l')
                ssmode << setUserLimit(chan, addminus, ssmode.str(), paramsplit, arg, pos, fd);
            else
                notifyUsers(ERR_UNKNOWNMODE(user->getNickname(), chan->getChannelName(), modeString[i]), fd);
        }
    }
    

    std::string chain = ssmode.str();
    if (chain.empty())
        return;
    chan->broadcastMessage(RPL_CHANGEMODE(user->getHostname(), chan->getChannelName(), ssmode.str(), arg));
}

void Server::parseChannelMode(std::string message, std::string &channelname, std::string &modeString, std::string &param)
{
    std::istringstream ss(message);

    // Extract channel name and mode string
    ss >> channelname;
    ss >> modeString;

    // Construct the prefix to skip in the message
    std::string combinedPrefix = channelname + " " + modeString;

    // Find the position where parameters start
    size_t nonspace = message.find_first_not_of(combinedPrefix + " \t\v");
    if (nonspace != std::string::npos) {
        param = message.substr(nonspace);
    } else {
        param = ""; // Ensure param is cleared if no additional parameters
    }
}

std::vector<std::string> Server::splitChannelMode(std::string param)
{
    std::vector<std::string> paramsplit;
    std::string line;
    std::istringstream ss(param);

    if (!param.empty() && param[0] == ':')
        param.erase(param.begin());
    while (std::getline(ss, line, ','))
    {
        paramsplit.push_back(line);
        line.clear();
    }
    return paramsplit;
}

std::string Server::updateMode(std::string ssmode, char addminus, char mode)
{
    std::stringstream ss;

    ss.clear();
    char nul = '\0';
    for (size_t i = 0; i < ssmode.size(); i++)
    {
        if (ssmode[i] == '+' || ssmode[i] == '-')
            nul = ssmode[i];
    }
    if (nul != addminus)
        ss << addminus << mode;
    else
        ss << mode;
    return ss.str();
}

std::string Server::inviteOnly(Channel *channel, char addminus, std::string ssmode)
{
    std::string str;

    str.clear();
    if (addminus == '+' && !channel->getChannelModeOption(0))
    {
        channel->setChannelMode(0, true);
        channel->setInviteOnlyStatus(1);
        str = updateMode(ssmode, addminus, 'i');
    }
    if (addminus == '-' && channel->getChannelModeOption(0))
    {
        channel->setChannelMode(0, false);
        channel->setInviteOnlyStatus(0);
        str = updateMode(ssmode, addminus, 'i');
    }
    return str;
}

std::string Server::topicRestriction(Channel *channel, char addminus, std::string ssmode)
{
    std::string str;

    str.clear();
    if (addminus == '+' && !channel->getChannelModeOption(1))
    {
        channel->setChannelMode(1, true);
        channel->setTopicRestriction(true);
        str = updateMode(ssmode, addminus, 't');
    }
    else if (addminus == '-' && channel->getChannelModeOption(1))
    {
        channel->setChannelMode(1, false);
        channel->setTopicRestriction(false);
        str = updateMode(ssmode, addminus, 't');
    }
    return str;
}

std::string Server::channelPassword(Channel *channel, char addminus, std::string ssmode, 
                                    std::vector<std::string> paramsplit, std::string &arg, size_t &pos, int fd) {
    std::string result = "";
    std::string password = "";

    // Check if a parameter is available
    if (pos >= paramsplit.size()) {
        notifyUsers(ERR_NEEDMODEPARAM(channel->getChannelName(), "(k)"), fd);
        return result;
    }

    password = paramsplit[pos++];

    // Validate the password
    if (!isValidChannelPassword(password)) {
        notifyUsers(ERR_NEEDMODEPARAM(channel->getChannelName(), "(k)"), fd);
        return result;
    }

    if (addminus == '+') {
        // Add the password to the channel
        channel->setChannelMode(2, true);
        channel->setChannelPassword(password);

        // Append password to argument list
        if (!arg.empty()) {
            arg += " ";
        }
        arg += password;

        // Update the mode string
        result = updateMode(ssmode, addminus, 'k');
    } else if (addminus == '-' && channel->getChannelModeOption(2)) {
        // Remove the password if it matches the existing password
        if (password == channel->getChannelPassword()) {
            channel->setChannelMode(2, false);
            channel->setChannelPassword("");

            // Update the mode string
            result = updateMode(ssmode, addminus, 'k');
        } else {
            // Notify if the password does not match
            notifyUsers(ERR_KEYEXIST(channel->getChannelName()), fd);
        }
    }

    return result;
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

std::string Server::operatorPrivilege(Channel *channel, char addminus, std::string ssmode, 
                                      std::vector<std::string> paramsplit, std::string &arg, size_t &pos, int fd) {
    std::string result = "";
    std::string user = "";

    // Check if a parameter is provided
    if (pos >= paramsplit.size()) {
        notifyUsers(ERR_NEEDMODEPARAM(channel->getChannelName(), "(o)"), fd);
        return result;
    }

    user = paramsplit[pos++];

    // Check if the user is in the channel
    if (!channel->isUserInChannel(user)) {
        notifyUsers(ERR_NOSUCHNICKCHAN(channel->getChannelName(), user), fd);
        return result;
    }

    // Handle adding or removing operator privileges
    if (addminus == '+') {
        channel->setChannelMode(3, true);
        if (channel->promoteUserToOperator(user)) {
            result = updateMode(ssmode, addminus, 'o');
            if (!arg.empty()) {
                arg += " ";
            }
            arg += user;
        }
    } else if (addminus == '-') {
        channel->setChannelMode(3, false);
        if (channel->demoteOperatorToUser(user)) {
            result = updateMode(ssmode, addminus, 'o');
            if (!arg.empty()) {
                arg += " ";
            }
            arg += user;
        }
    }

    return result;
}

bool Server::isValidUserLimit(std::string &limit) {
    // Check if the string contains only digits
    for (size_t i = 0; i < limit.size(); ++i) {
        if (!std::isdigit(limit[i])) {
            return false;
        }
    }

    // Convert the string to an integer and ensure it is greater than 0
    int userLimit = std::atoi(limit.c_str());
    return userLimit > 0;
}

std::string Server::setUserLimit(Channel *channel, char addminus, std::string ssmode,
                                std::vector<std::string> paramsplit, std::string &arg,
                                size_t &pos, int fd) {
    std::string res = "";
    std::string limit = "";

    if (addminus == '+') {
        // Validate parameter availability
        if (pos >= paramsplit.size()) {
            notifyUsers(ERR_NEEDMODEPARAM(channel->getChannelName(), "(l)"), fd);
            return res;
        }

        // Extract and validate the limit
        limit = paramsplit[pos++];
        if (!isValidUserLimit(limit)) {
            notifyUsers(ERR_INVALIDMODEPARAM(channel->getChannelName(), "(l)"), fd);
            return res;
        }

        channel->setChannelMode(4, true);
        channel->setUserLimit(std::atoi(limit.c_str()));

        if (!arg.empty())
            arg += " ";

        arg += limit;
        res = updateMode(ssmode, addminus, 'l');
        } else if (addminus == '-' && channel->getChannelModeOption(4)) {
        channel->setChannelMode(4, false);
        channel->setUserLimit(0);
        res = updateMode(ssmode, addminus, 'l');
    }
    return res;
}
