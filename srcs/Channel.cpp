#include "../includes/channel.hpp"

// Default Channel class constructor
// Channel::Channel()
// {
//     this->channelName = "";
//     this->topicName = "";
//     this->created_at = "";
//     this->topic = 0;
//     this->key = 0;
//     this->userLimit = 0;
//     this->inviteOnly = 0;
//     this->topicRestriction = false;
    
//     char mode[5] = {'i', 't', 'k', 'o', 'l'};
//     for (int i = 0; i < 5; i++)
//         channelModes.push_back(std::make_pair(mode[i], false));
// }

Channel::Channel()
    : topic(false), key(false), userLimit(0), inviteOnly(false), topicRestriction(false) {
    created_at = std::to_string(std::time(NULL));
    char modes[] = {'i', 't', 'k', 'o', 'l'};
    for (size_t i = 0; i < 5; ++i) {
        channelModes.push_back(std::make_pair(modes[i], false));
    }
}

// Channel class copy constructor
Channel::Channel(const Channel &src) {
    *this = src;
}

// Channel class assignment operator
Channel &Channel::operator=(const Channel &rhs) {
    if (this != &rhs)
    {
        channelName = rhs.channelName;
        topicName = rhs.topicName;
        password = rhs.password;
        topic = rhs.topic;
        key = rhs.key;
        userLimit = rhs.userLimit;
        sock_user = rhs.sock_user;
        admin = rhs.admin;
        inviteOnly = rhs.inviteOnly;
        topicRestriction = rhs.topicRestriction;
        created_at = rhs.created_at;
        channelModes = rhs.channelModes;
    }
    return (*this);
}

// Channel class destructor
Channel::~Channel() {}

// Returns the channel name
std::string Channel::getChannelName () const {
    return (channelName);
}

// Returns the topic name
std::string Channel::getTopicName() const {
    return (topicName);
}

// Returns the channel password
std::string Channel::getChannelPassword() const {
    return (this->password);
}

// Generate a list of channel member users and return it
std::string Channel::getUserList() {
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

// Returns the channel creation date
std::string Channel::getCreationDate() const {
    return (this->created_at);
}

// Returns the channel modes as a string
std::string Channel::getChannelModes() const {
    std::string mode;

    for (size_t i = 0; i < channelModes.size(); i++)
    {
        if (channelModes[i].first != 'o' && channelModes[i].second)
            mode.push_back(channelModes[i].first);
    }
    if (!mode.empty())
        mode.insert(mode.begin(), '+');

    return (mode);
}

// Returns a reference to the list of non-operator users of the channel
std::vector<User> &Channel::getUsers() {
    return (sock_user);
}

// Returns a reference to the list of channel operators
std::vector<User> &Channel::getOperators() {
    return (admin);
}
// Returns a pointer to the user with the file descriptor given in sock_user
User *Channel::getUserByFd(int fd) {
    for (std::vector<User>::iterator it = sock_user.begin(); it != sock_user.end(); ++it)
    {
        if (it->getFduser() == fd)
            return &(*it);
    }
    return (NULL);
}
// Returns a pointer to the operator with the file descriptor given in admin
User *Channel::getOperatorByFd(int fd) {
    for (std::vector<User>::iterator it = admin.begin(); it != admin.end(); ++it)
    {
        if (it->getFduser() == fd)
            return &(*it);
    }
    return (NULL);
}
// Search for a user by name in sock_user and admin
User *Channel::getFindUserByName(const std::string name) {
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
// Returns the status of the invitation (0 or 1);
int Channel::getInviteOnlyStatus() {
    return this->inviteOnly;
}
// Return the state of the subject (0 or 1);
int Channel::getTopicStatus() const {
    return this->topic;
}
// Returns the channel key (password);
int Channel::getKeyStatus() const {
    return this->key;
}
// Returns the number of users limit
int Channel::getUserLimit() const {
    return this->userLimit;
}
// Returns the restrictions on the subject (true or false)
bool Channel::getTopicRestriction() const {
    return (this->topicRestriction);
}
// Returns the channel mode option specified by index i
bool Channel::getChannelModeOption(size_t i) const {
    return (channelModes[i].second);
}
// Returns a list of pointers to channel users
std::vector<User *> Channel::getUserPointers() {
    std::vector<User *> pointers;
    for (size_t i = 0; i < sock_user.size(); i++)
    {
        pointers.push_back(&sock_user[i]);
    }
    return (pointers);
}
// Set the channel name
void Channel::setChannelName(std::string name) {
    this->channelName = name;
}
// Set the topic name
void Channel::setTopicName(const std::string topic) {
    this->topicName = topic;
}
// Set the channel password
void Channel::setChannelPassword(const std::string password) {
    this->password = password;
}
// Set the invitation status (0 or 1);
void Channel::setInviteOnlyStatus(int inviteOnly) {
    this->inviteOnly = inviteOnly;
}
// Set restrictions on the subject (true or false)
void Channel::setTopicRestriction(bool restriction) {
    this->topicRestriction = restriction;
}
// Set the state of the subject (0 or 1);
void Channel::setTopicStatus(int topic) {
    this->topic = topic;
}
// Set the channel key (password);
void Channel::setKeyStatus(int key) {
    this->key = key;
}
// Set the limit on the number of users
void Channel::setUserLimit(int limit) {
    this->userLimit = limit;
}
// Set the channel mode option specified by index i
void Channel::setChannelMode(size_t i, bool mode) {
    channelModes[i].second = mode;
}
// Set the channel creation date
void Channel::setCreationDate() {
    std::time_t _time = std::time(NULL);
    std::ostringstream oss;
    oss << _time;
    this->created_at = std::string(oss.str());
}
// Remove a user from the channel based on their file descriptor
void Channel::removeUserByFd(int fd) {
    for (std::vector<User>::iterator it = sock_user.begin(); it != sock_user.end(); ++it) {
        if (it->getFduser() == fd) {
            sock_user.erase(it);
            return;
        }
    }
}
// Remove an operator from the pipe based on its file descriptor
void Channel::removeOperatorByFd(int fd) {
    for (std::vector<User>::iterator it = admin.begin(); it != admin.end(); ++it) {
        if (it->getFduser() == fd) {
            admin.erase(it);
            return;
        }
    }
}
// Add a user to the list of channel members
void Channel::addUser(const User user) {
    sock_user.push_back(user);
}
// Add a user to the list of channel operators
void Channel::addOperatorOnChannel(const User user) {
    admin.push_back(user);
}
// Check if the channel name is correct
void Channel::checkChannelName(std::string channelName) {
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
// Returns the total number of users in the channel (operators + normal users)
size_t Channel::getUserCount() {
    size_t num = this->sock_user.size() + this->admin.size();
    return (num);
}
// Check if a user with the given name is present in the channel
bool Channel::isUserInChannel(std::string &name) {
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
// Check if a user with the given name is an operator in the channel
bool Channel::isUserOperator(std::string &name) {
    for (std::vector<User>::iterator it = admin.begin(); it != admin.end(); ++it)
    {
        if (it->getNickname() == name)
            return (true);
    }
    return (false);
}
// Check if the channel has operators
bool Channel::hasOperators() {
    if (admin.empty())
        return (false);
    return (true);
}
// Send a message to all users and operators present in the channel
void Channel::broadcastMessage(std::string reply) {
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
// Send a message to all users and operators present in the channel except one
void Channel::broadcastMessage2(std::string reply, int fd) {
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
// Promotes the first non-operator user to operator
void Channel::promoteFirstUserToOperator() {
    admin.push_back(sock_user[0]);
    sock_user.erase(sock_user.begin());
}
// Promotes a specific user to operator
// bool Channel::promoteUserToOperator(std::string& name) {
//     size_t i = 0;
//     for (; i < sock_user.size(); i++)
//     {
//         if (sock_user[i].getNickname() == name)
//             break;
//     }
//     if (i < sock_user.size())
//     {
//         admin.push_back(sock_user[i]);
//         sock_user.erase(i + sock_user.begin());
//         return true;
//     }
//     return false;
// }

bool Channel::promoteUserToOperator(const std::string& name) {
    for (std::vector<User>::iterator it = sock_user.begin(); it != sock_user.end(); ++it) {
        if (it->getNickname() == name) {
            admin.push_back(*it);
            sock_user.erase(it);
            return true;
        }
    }
    return false;
}

// Reduce a specific operator to non-operator user
// bool Channel::demoteOperatorToUser(std::string& name) {
//     size_t i = 0;
//     for (; i < admin.size(); i++)
//     {
//         if (admin[i].getNickname() == name)
//             break;
//     }
//     if (i < admin.size())
//     {
//         sock_user.push_back(admin[i]);
//         admin.erase(i + admin.begin());
//         return true;
//     }
//     return false;
// }

bool Channel::demoteOperatorToUser(const std::string& name) {
    for (std::vector<User>::iterator it = admin.begin(); it != admin.end(); ++it) {
        if (it->getNickname() == name) {
            sock_user.push_back(*it);
            admin.erase(it);
            return true;
        }
    }
    return false;
}

// Send a message to the channel on behalf of a user
void Channel::notifyUsers(std::string msg, User &author) {
    std::cout << "channel:: notifyUsers" << std::endl;
    (void)msg;
    (void)author;
}
