#include "../includes/channel.hpp"

Channel::Channel()
{
    channelName = "";
    topicName = "";
    created_at = "";
    topic = 0;
    key = 0;
    limit = 0;
    invitOnly = 0;
    topicRestriction = false;
    
    char mode[5] = {'i', 't', 'k', 'o', 'l'};
    for (int i = 0; i < 5; i++)
        channelMode.push_back(std::make_pair(mode[i], false));
}

Channel::Channel(Channel const &src)
{
    *this = src;
}

Channel::~Channel()
{
}

Channel &Channel::operator=(Channel const &rhs)
{
    if (this != &rhs)
    {
        channelName = rhs.channelName;
        topicName = rhs.topicName;
        password = rhs.password;
        topic = rhs.topic;
        key = rhs.key;
        limit = rhs.limit;
        sock_user = rhs.sock_user;
        admin = rhs.admin;
        invitOnly = rhs.invitOnly;
        topicRestriction = rhs.topicRestriction;
        created_at = rhs.created_at;
        channelMode = rhs.channelMode;
    }
    return (*this);
}

std::string Channel::getChannelName()
{
    return (channelName);
}

std::string Channel::getTopicName()
{
    return (topicName);
}

std::string Channel::getChannelPassword()
{
    return (password);
}

std::string Channel::getUserList()
{
    std::string list;

    for (size_t i = 0; i < admin.size(); i++)
    {
        list += "@" + admin[i].getNickname();
        if ((i + 1) < admin.size())
            list += " ";
    }
    if (sock_user.size())
        list += " ";
    for (size_t i = 0; i < sock_user.size(); i++)
    {
        list += sock_user[i].getNickname();
        if ((i + 1) < sock_user.size())
            list += " ";
    }

    return (list);
}

std::string Channel::getCreationDate()
{
    return (created_at);
}

std::string Channel::getChannelModes()
{
    std::string mode;

    for (size_t i = 0; i < channelMode.size(); i++)
    {
        if (channelMode[i].first != 'o' && channelMode[i].second)
            mode.push_back(channelMode[i].first);
    }
    if (!mode.empty())
        mode.insert(mode.begin(), '+');

    return (mode);
}

std::vector<User> &Channel::getUsers()
{
    return (sock_user);
}

std::vector<User> &Channel::getOperators()
{
    return (admin);
}

User *Channel::getUserByFd(int fd)
{
    for (std::vector<User>::iterator it = sock_user.begin(); it != sock_user.end(); ++it)
    {
        if (it->getFduser() == fd)
            return &(*it);
    }
    return (NULL);
}

User *Channel::getOperatorByFd(int fd)
{
    for (std::vector<User>::iterator it = admin.begin(); it != admin.end(); ++it)
    {
        if (it->getFduser() == fd)
            return &(*it);
    }
    return (NULL);
}

User *Channel::getFindUserByName(std::string name)
{
    for (std::vector<User>::iterator it = sock_user.begin(); it != sock_user.end(); ++it)
    {
        if (it->getNickname() == name)
            return &(*it);
    }
    for (std::vector<User>::iterator it = admin.begin(); it != admin.end(); ++it)
    {
        if (it->getNickname() == name)
            return &(*it);
    }
    return (NULL);
}

int Channel::getInviteOnlyStatus()
{
    return invitOnly;
}

int Channel::getTopicStatus()
{
    return topic;
}

int Channel::getKeyStatus()
{
    return key;
}

int Channel::getUserLimit()
{
    return limit;
}

bool Channel::getTopicRestriction() const
{
    return (topicRestriction);
}

bool Channel::getChannelModeOption(size_t i)
{
    return (channelMode[i].second);
}

std::vector<User *> Channel::getUserPointers()
{
    std::vector<User *> pointers;
    for (size_t i = 0; i < sock_user.size(); i++)
    {
        pointers.push_back(&sock_user[i]);
    }
    return (pointers);
}

void Channel::setChannelName(std::string name)
{
    channelName = name;
}

void Channel::setTopicName(std::string topic)
{
    topicName = topic;
}

void Channel::setChannelPassword(std::string password)
{
    this->password = password;
}

void Channel::setInviteOnlyStatus(int invitOnly)
{
    this->invitOnly = invitOnly;
}

void Channel::setTopicRestriction(bool restriction)
{
    this->topicRestriction = restriction;
}

void Channel::setTopicStatus(int topic)
{
    this->topic = topic;
}

void Channel::setKeyStatus(int key)
{
    this->key = key;
}

void Channel::setUserLimit(int limit)
{
    this->limit = limit;
}

void Channel::setChannelMode(size_t i, bool mode)
{
    channelMode[i].second = mode;
}

void Channel::setCreationDate()
{
    std::time_t _time = std::time(NULL);
    std::ostringstream oss;
    oss << _time;
    created_at = std::string(oss.str());
}

void Channel::removeUserByFd(int fd)
{
    for (std::vector<User>::iterator it = sock_user.begin(); it != sock_user.end(); ++it)
    {
        if (it->getFduser() == fd)
        {
            sock_user.erase(it);
            break;
        }
    }
}

void Channel::removeOperatorByFd(int fd)
{
    for (std::vector<User>::iterator it = admin.begin(); it != admin.end(); ++it)
    {
        if (it->getFduser() == fd)
        {
            admin.erase(it);
            break;
        }
    }
}

void Channel::addUser(User user)
{
    sock_user.push_back(user);
}

void Channel::addOperatorOnChannel(User user)
{
    admin.push_back(user);
}

void Channel::checkChannelName(std::string channelName)
{
    if (channelName.empty() || channelName.size() < 2)
        throw std::runtime_error("Incorrect channel name");
    if (channelName[0] != '&')
        throw std::runtime_error("Incorrect channel name, it must begin with &");
    for (size_t i = 0; i < channelName.size(); ++i)
    {
        if (channelName[i] == ' ' || channelName[i] == 0x07 || channelName[i] == ',')
            throw std::runtime_error("Incorrect channel name");
    }
}

size_t Channel::getUserCount()
{
    size_t num = sock_user.size() + admin.size();
    return (num);
}

bool Channel::isUserInChannel(std::string &name)
{
    for (size_t i = 0; i < sock_user.size(); i++)
    {
        if (sock_user[i].getNickname() == name)
            return true;
    }
    for (size_t i = 0; i < admin.size(); i++)
    {
        if (admin[i].getNickname() == name)
            return true;
    }
    return false;
}

bool Channel::isUserOperator(std::string &name)
{
    for (std::vector<User>::iterator it = admin.begin(); it != admin.end(); ++it)
    {
        if (it->getNickname() == name)
            return (true);
    }
    return (false);
}

bool Channel::hasOperators()
{
    if (admin.empty())
        return (false);
    return (true);
}

void Channel::broadcastMessage(std::string reply)
{
    for (size_t i = 0; i < admin.size(); i++)
    {
        if (send(admin[i].getFduser(), reply.c_str(), reply.size(), 0) == -1)
            std::cerr << "send() failed" << std::endl;
    }
    for (size_t i = 0; i < sock_user.size(); i++)
    {
        if (send(sock_user[i].getFduser(), reply.c_str(), reply.size(), 0) == -1)
            std::cerr << "send() failed" << std::endl;
    }
}

void Channel::broadcastMessage2(std::string reply, int fd)
{
    for (size_t i = 0; i < admin.size(); i++)
    {
        if (admin[i].getFduser() != fd)
        {
            if (send(admin[i].getFduser(), reply.c_str(), reply.size(), 0) == -1)
                std::cerr << "send() failed" << std::endl;
        }
    }
    for (size_t i = 0; i < sock_user.size(); i++)
    {
        if (sock_user[i].getFduser() != fd)
        {
            if (send(sock_user[i].getFduser(), reply.c_str(), reply.size(), 0) == -1)
                std::cerr << "send() failed" << std::endl;
        }
    }
}

void Channel::promoteFirstUserToOperator()
{
    admin.push_back(sock_user[0]);
    sock_user.erase(sock_user.begin());
}

bool Channel::promoteUserToOperator(std::string& name)
{
    size_t i = 0;
    for (; i < sock_user.size(); i++)
    {
        if (sock_user[i].getNickname() == name)
            break;
    }
    if (i < sock_user.size())
    {
        admin.push_back(sock_user[i]);
        sock_user.erase(i + sock_user.begin());
        return true;
    }
    return false;
}

bool Channel::demoteOperatorToUser(std::string& name)
{
    size_t i = 0;
    for (; i < admin.size(); i++)
    {
        if (admin[i].getNickname() == name)
            break;
    }
    if (i < admin.size())
    {
        sock_user.push_back(admin[i]);
        admin.erase(i + admin.begin());
        return true;
    }
    return false;
}

void Channel::notifyUsers(std::string msg, User &author)
{
    std::cout << "channel:: notifyUsers" << std::endl;
    (void)msg;
    (void)author;
}
