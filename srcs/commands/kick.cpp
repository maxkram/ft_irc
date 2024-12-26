#include "../../includes/server.hpp"

void Server::KICK(std::string message, int fd)
{
    std::vector<std::string> param;
    std::string comment, user;
    
    User *cash = getClientByFd(fd);

    comment = splitKickParams(message, param, user, fd);

    if (user.empty()) {
        notifyClient3(461, cash->getNickname(), cash->getFduser(), " :Not enough parameters\r\n");
        return;
    }
    for (size_t i = 0; i < param.size(); ++i) {
        Channel *chan = getChannel(param[i]);
        if (!chan) {
            notifyClient2(403, cash->getNickname(), "#" + param[i], cash->getFduser(), " :No such channel\r\n");
            continue;
        }

        if (!chan->getUserByFd(fd) && !chan->getOperatorByFd(fd)) {
            notifyClient2(442, cash->getNickname(), "#" + param[i], cash->getFduser(), " :You're not on that channel\r\n");
            continue;
        }

        if (!chan->getOperatorByFd(fd)) {
            notifyClient2(482, cash->getNickname(), "#" + param[i], cash->getFduser(), " :You're not channel operator\r\n");
            continue;
        }

        if (chan->getFindUserByName(user))
        {
            std::stringstream ss;
            ss << ":" << cash->getNickname() << "!~" << cash->getUser() << "@localhost KICK #" << param[i] << " " << user;
            if (!comment.empty())
                ss << " :" << comment << "\r\n";
            else
                ss << "\r\n";
            chan->broadcastMessage(ss.str());
            if (chan->getOperatorByFd(chan->getFindUserByName(user)->getFduser()))
                chan->removeOperatorByFd(chan->getFindUserByName(user)->getFduser());
            else
                chan->removeUserByFd(chan->getFindUserByName(user)->getFduser());
            if (chan->getUserCount() == 0)
                channel.erase(channel.begin() + i);
        }
        else {
            notifyClient2(441, cash->getNickname(), "#" + param[i], cash->getFduser(), " :They aren't on that channel\r\n");
            continue;
        }

    }
}

std::string Server::splitKickParams(std::string message, std::vector<std::string> &param, std::string &user, int fd)
{
    std::string comment = kickReason(message, param);
    std::string tmp;

    if (param.size() < 3)
        return std::string("");
    // Extract channels and user
    param.erase(param.begin());
    std::string str = param[0];
    user = param[1];
    param.clear();
    // Split channels by commas
    for (size_t i = 0; i < str.size(); ++i) {
        if (str[i] == ',') {
            param.push_back(tmp);
            tmp.clear();
        }
        else
            tmp += str[i];
    }
    param.push_back(tmp);
    // Remove leading ':' from comment, if present
    for (size_t i = 0; i < param.size(); ++i) {
        if (param[i].empty())
            param.erase(param.begin() + --i);
    }
    if (comment[0] == ':')
        comment.erase(comment.begin());
    else {
        for (size_t i = 0; i < comment.size(); i++) {
            if (comment[i] == ' ') {
                comment = comment.substr(0, i);
                break;
            }
        }
    }
    for (size_t i = 0; i < param.size(); ++i) {
        if (*(param[i].begin()) == '#')
            param[i].erase(param[i].begin());
        else {
            notifyClient2(403, getClientByFd(fd)->getNickname(), param[i], getClientByFd(fd)->getFduser(), " :No such channel\r\n");
            param.erase(param.begin() + i--);
        }
    }
    return comment;
}

std::string Server::kickReason(std::string &message, std::vector<std::string> &param)
{
    std::stringstream ss(message);
    std::string str, comment;

    int count = 3;
    while (ss >> str && count--)
        param.push_back(str);
    if (param.size() != 3)
        return std::string("");
    kickReason2(message, param[2], comment);
    return comment;
}

void Server::kickReason2(const std::string &message, const std::string &tofind, std::string &comment)
{
    size_t pos = message.find(tofind);

    if (pos != std::string::npos) {
        comment = message.substr(pos + tofind.size());
        size_t firstNonSpace = comment.find_first_not_of(' ');
        if (firstNonSpace != std::string::npos)
            comment = comment.substr(firstNonSpace);
    }
    else {
        comment.clear();
    }
}