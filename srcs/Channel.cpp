#include "../includes/channel.hpp"

Channel::Channel()
    : channelName(""),
      topicName(""),
      creationTime(""),
      topic(0),
      key(0),
      limit(0),
      invitOnly(0),
      restrictTopic(false)
{
    char modes[] = {'i', 't', 'k', 'o', 'l'};
    for (size_t i = 0; i < sizeof(modes) / sizeof(modes[0]); ++i)
    {
        channelMode.push_back(std::pair<char, bool>(modes[i], false));
    }
}


Channel::Channel(Channel const &src) { *this = src; }

Channel::~Channel(){}

Channel &Channel::operator=(const Channel &rhs)
{
    if (this == &rhs)
        return *this;

    // Assign string members
    channelName = rhs.channelName;
    topicName = rhs.topicName;
    password = rhs.password;
    creationTime = rhs.creationTime;

    // Assign integral and boolean members
    topic = rhs.topic;
    key = rhs.key;
    limit = rhs.limit;
    invitOnly = rhs.invitOnly;
    restrictTopic = rhs.restrictTopic;

    // Assign container members
    regularUsers = rhs.regularUsers;
    admin = rhs.admin;
    channelMode = rhs.channelMode;

    return *this;
}

std::string Channel::getChannelName() { return (channelName); }

std::string Channel::getTopicName() { return (topicName); }

std::string Channel::getChannelPassword() { return (this->password); }

std::string Channel::getUserList()
{
    std::string list;
    bool first = true; // Flag to track if it's the first nickname being added

    // Helper function to append nicknames
    for (size_t i = 0; i < admin.size(); i++)
    {
        if (!first) // Add a space before subsequent nicknames
            list += " ";
        list += "@" + admin[i].getNickname();
        first = false;
    }

    for (size_t i = 0; i < regularUsers.size(); i++)
    {
        if (!first) // Add a space before subsequent nicknames
            list += " ";
        list += regularUsers[i].getNickname();
        first = false;
    }

    return list;
}

std::string Channel::getCreationDate() { return (this->creationTime); }

std::string Channel::getChannelModes()
{
    std::string mode;
    bool hasModes = false;

    for (size_t i = 0; i < channelMode.size(); i++) {
        if (channelMode[i].first != 'o' && channelMode[i].second) {
            mode.push_back(channelMode[i].first);
            hasModes = true;
        }
    }

    // Prepend '+' if any mode was added
    if (hasModes)
        mode.insert(mode.begin(), '+');

    return mode;
}

std::vector<User> &Channel::getUsers() { return (regularUsers); }

std::vector<User> &Channel::getOperators() { return (admin); }

User *Channel::getUserByFd(int fd)
{
    for (size_t i = 0; i < regularUsers.size(); ++i)
    {
        if (regularUsers[i].getFduser() == fd)
            return &regularUsers[i];
    }
    return NULL;
}

User *Channel::getOperatorByFd(int fd) {
    for (size_t i = 0; i < admin.size(); ++i) {
        if (admin[i].getFduser() == fd)
            return &admin[i];
    }
    return NULL;
}

User *Channel::getFindUserByName(std::string name) {
    for (size_t i = 0; i < regularUsers.size(); ++i) {
        if (regularUsers[i].getNickname() == name)
            return &regularUsers[i];
    }
    for (size_t i = 0; i < admin.size(); ++i) {
        if (admin[i].getNickname() == name)
            return &admin[i];
    }
    return NULL;
}

int Channel::isInviteOnly() {
    return invitOnly;
}

int Channel::isTopicSet() {
    return topic;
}

int Channel::hasPassword() {
    return key;
}

int Channel::getUserLimit() {
    return limit;
}

bool Channel::isTopicRestricted() const {
    return restrictTopic;
}

bool Channel::isModeEnabled(size_t i) {
    return (channelMode[i].second);
}

std::vector<User *> Channel::getUserPointers()
{
    std::vector<User *> pointers;
    pointers.reserve(regularUsers.size()); // Reserve memory for better performance

    for (size_t i = 0; i < regularUsers.size(); ++i) {
        pointers.push_back(&regularUsers[i]);
    }

    return pointers;
}

void Channel::setChannelName(std::string name) {
    channelName = name;
}

void Channel::setTopic(std::string topic) {
    topicName = topic;
}

void Channel::setPassword(std::string password) {
    this->password = password;
}

void Channel::setInviteOnly(int invitOnly) {
    this->invitOnly = invitOnly;
}

void Channel::setTopicRestriction(bool restriction) {
    restrictTopic = restriction;
}

void Channel::setTopicFlag(int topic) {
    this->topic = topic;
}

void Channel::setKeyStatus(int key) {
    this->key = key;
}

void Channel::setUserLimit(int limit) {
    this->limit = limit;
}

void Channel::setChannelMode(size_t i, bool mode) {
    channelMode[i].second = mode;
}

void Channel::setCreationDate() {
    std::time_t currentTime = std::time(NULL);
    std::ostringstream oss;
    oss << currentTime;
    creationTime = oss.str(); // Direct assignment without redundant std::string creation
}

void Channel::removeUserByFd(int fd) {
    for (size_t i = 0; i < regularUsers.size(); ++i) {
        if (regularUsers[i].getFduser() == fd) {
            regularUsers.erase(regularUsers.begin() + i);
            break;
        }
    }
}

void Channel::removeOperatorByFd(int fd) {
    for (size_t i = 0; i < admin.size(); ++i) {
        if (admin[i].getFduser() == fd) {
            admin.erase(admin.begin() + i);
            break;
        }
    }
}


void Channel::addRegularUser(User user) {
    regularUsers.push_back(user);
}

void Channel::addOperator(User user) {
    admin.push_back(user);
}

void Channel::validateName(std::string channelName) {
    if (channelName.empty() || channelName.size() < 2)
        throw std::runtime_error("Channel name must be at least 2 characters long.");
    
    if (channelName[0] != '&')
        throw std::runtime_error("Channel name must begin with '&'.");
    
    for (std::string::size_type i = 0; i < channelName.size(); ++i) {
        char c = channelName[i];
        if (c == ' ' || c == '\a' || c == ',')
            throw std::runtime_error("Channel name contains invalid characters.");
    }
}

size_t Channel::getUserCount() {
    return regularUsers.size() + admin.size();
}

bool Channel::isUserInChannel(std::string &name) {
    for (size_t i = 0; i < regularUsers.size(); ++i) {
        if (regularUsers[i].getNickname() == name)
            return true;
    }

    for (size_t i = 0; i < admin.size(); ++i) {
        if (admin[i].getNickname() == name)
            return true;
    }

    return false;
}

bool Channel::isUserOperator(std::string &name)
{
    for (size_t i = 0; i < admin.size(); ++i) {
        if (admin[i].getNickname() == name)
            return true;
    }
    return false;
}

bool Channel::hasOperators() {
    return !admin.empty();
}

void Channel::broadcastMessage(std::string reply)
{
    // Helper function to send a message to a group of users
    for (size_t i = 0; i < admin.size(); ++i) {
        if (send(admin[i].getFduser(), reply.c_str(), reply.size(), 0) == -1) {
            std::cerr << "send() failed for admin: " << admin[i].getFduser() << std::endl;
        }
    }

    for (size_t i = 0; i < regularUsers.size(); ++i) {
        if (send(regularUsers[i].getFduser(), reply.c_str(), reply.size(), 0) == -1) {
            std::cerr << "send() failed for user: " << regularUsers[i].getFduser() << std::endl;
        }
    }
}

void Channel::sendMessageToAllExcept(std::string reply, int fd)
{
    // Helper function to send a message to all users except one
    for (size_t i = 0; i < admin.size(); ++i) {
        if (admin[i].getFduser() != fd)
        {
            if (send(admin[i].getFduser(), reply.c_str(), reply.size(), 0) == -1) {
                std::cerr << "send() failed for admin: " << admin[i].getFduser() << std::endl;
            }
        }
    }

    for (size_t i = 0; i < regularUsers.size(); ++i) {
        if (regularUsers[i].getFduser() != fd) {
            if (send(regularUsers[i].getFduser(), reply.c_str(), reply.size(), 0) == -1) {
                std::cerr << "send() failed for user: " << regularUsers[i].getFduser() << std::endl;
            }
        }
    }
}

void Channel::promoteFirstUser() {
    if (!regularUsers.empty()) { // Check if there are users in regularUsers
        admin.push_back(regularUsers[0]);
        regularUsers.erase(regularUsers.begin());
    }
}

bool Channel::promoteToOperator(std::string &name) {
    for (size_t i = 0; i < regularUsers.size(); ++i) {
        if (regularUsers[i].getNickname() == name) {
            admin.push_back(regularUsers[i]);
            regularUsers.erase(regularUsers.begin() + i); // Adjusted to avoid unnecessary addition
            return true;
        }
    }
    return false;
}

bool Channel::demoteOperatorToUser(std::string &name) {
    for (size_t i = 0; i < admin.size(); ++i) {
        if (admin[i].getNickname() == name) {
            regularUsers.push_back(admin[i]);
            admin.erase(admin.begin() + i); // Simplified erase operation
            return true;
        }
    }
    return false;
}

void Channel::notifyUsers(std::string msg, User &author)
{
    std::cout << "channel:: notifyUsers" << std::endl;
    (void)msg;
    (void)author;
}