#include "../includes/user.hpp"

User::User()
{
    this->fdUser = -1;
    this->nickname = "";
    this->user = "";
    this->ip = "";
    this->buff = "";
    this->realname = "";
    this->registered = false;
    this->connected = false;
    this->isOp = false;
    this->chanFounder = false;
    time(&this->created);
    char modes[2] = {'i', 'o'};
    for (int i = 0; i < 2; i++)
        userModes.push_back(std::make_pair(modes[i], false));
}

User::User(int fd, std::string nickname, std::string user)
    : fdUser(fd), nickname(nickname), user(user) {}

User::User(User const &obj)
{
    *this = obj;
}

User &User::operator=(User const &obj)
{
    if (this != &obj)
    {
        this->fdUser = obj.fdUser;
        this->nickname = obj.nickname;
        this->user = obj.user;
        this->ip = obj.ip;
        this->buff = obj.buff;
        this->registered = obj.registered;
        this->connected = obj.connected;
        this->isOp = obj.isOp;
        this->userModes = obj.userModes;
        this->realname = obj.realname;
        this->chanFounder = obj.chanFounder;
        this->created = obj.created;
    }
    return *this;
}

bool User::operator==(User const &rhs)
{
    if (this->nickname == rhs.nickname)
        return (true);
    return (false);
}

User::~User()
{}

int User::getFduser()
{
    return (this->fdUser);
}

std::string &User::getNickname()
{
    return (this->nickname);
}

std::string User::getUser()
{
    return (user.empty() ? std::string("") : user);
}

std::string User::getIp()
{
    return (ip);
}

std::string User::getBuffer()
{
    return (buff);
}

std::string User::getHostname()
{
    std::string hostname = getNickname() + "!" + getUser();
    return (hostname);
}

std::string User::getRealname() const
{
    return (this->realname);
}

std::string User::getUserModes()
{
    std::string mode;
    for (size_t i = 0; i < userModes.size(); i++)
    {
        if (userModes[i].first != 'o' && userModes[i].second)
            mode.push_back(userModes[i].first);
    }
    if (!mode.empty())
        mode.insert(mode.begin(), '+');
    return (mode);
}

bool User::isRegistered()
{
    return (registered);
}

bool User::isConnected()
{
    return (this->connected);
}

bool User::isOperator()
{
    return (this->isOp);
}

bool User::isInvited(std::string &channel)
{
    for (size_t i = 0; i < this->invitation.size(); i++)
    {
        if (this->invitation[i] == channel)
            return (true);
    }
    return (false);
}

bool User::getUserModeOption(size_t i)
{
    return (userModes[i].second);
}

bool User::isChannelFounder()
{
    return (this->chanFounder);
}

time_t User::getCreationTime() const
{
    return (created);
}

void User::setFduser(int fd)
{
    this->fdUser = fd;
}

void User::setNickname(std::string &nickname)
{
    this->nickname = nickname;
}

void User::setUser(std::string &user)
{
    this->user = user;
}

void User::setIp(std::string ip)
{
    this->ip = ip;
}

void User::setBuffer(std::string recv)
{
    buff += recv;
}

void User::setRegistered(bool val)
{
    registered = val;
}

void User::setConnected(bool val)
{
    this->connected = val;
}

void User::setOperator(bool op)
{
    this->isOp = op;
}

void User::setChannelFounder(bool founder)
{
    this->chanFounder = founder;
}


void User::setUserMode(size_t i, bool mode)
{
    userModes[i].second = mode;
}

void User::setRealname(std::string realname)
{
    this->realname = realname;
}

void User::clearBuffer()
{
    buff.clear();
}

void User::addInvitation(std::string &channel)
{
    this->invitation.push_back(channel);
}

void User::removeInvitation(std::string &channel)
{
    for (size_t i = 0; i < this->invitation.size(); i++)
    {
        if (this->invitation[i] == channel)
        {
            this->invitation.erase(this->invitation.begin() + i);
            return;
        }
    }
}
