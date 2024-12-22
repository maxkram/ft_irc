#include "../includes/user.hpp"

// // Default constructor for the User class
// User::User()
// {
//     fdUser = -1;
//     nickname = "";
//     user = "";
//     ip = "";
//     buff = "";
//     realname = "";
//     registered = false;
//     connected = false;
//     isOp = false;
//     chanFounder = false;
//     time(&created);
//     char modes[2] = {'i', 'o'};
//     for (int i = 0; i < 2; i++)
//         userModes.push_back(std::make_pair(modes[i], false));
// }

// Default Constructor
User::User()
    : fdUser(-1), registered(false), connected(false), isOp(false), chanFounder(false) {
    time(&created);
    userModes.push_back(std::make_pair('i', false)); // Invisible mode
    userModes.push_back(std::make_pair('o', false)); // Operator mode
}

// // Parameterized constructor for the User class
// User::User(int fd, const std::string& nickname, const std::string& user)
//     : fdUser(fd), nickname(nickname), user(user) {}

// Parameterized Constructor
User::User(int fd, const std::string& nickname, const std::string& username)
    : fdUser(fd), nickname(nickname), user(username), registered(false), connected(false),
      isOp(false), chanFounder(false) {
    time(&created);
    userModes.push_back(std::make_pair('i', false)); // Invisible mode
    userModes.push_back(std::make_pair('o', false)); // Operator mode
}


// Copy constructor for the User class
User::User(const User &obj) {
    *this = obj;
}

// Assignment operator for the User class
User &User::operator=(const User &obj)
{
    if (this != &obj) {
        fdUser = obj.fdUser;
        nickname = obj.nickname;
        user = obj.user;
        ip = obj.ip;
        buff = obj.buff;
        registered = obj.registered;
        connected = obj.connected;
        isOp = obj.isOp;
        userModes = obj.userModes;
        realname = obj.realname;
        chanFounder = obj.chanFounder;
        created = obj.created;
    }
    return *this;
}

// Compares the nicknames of two users to determine if they are equal
bool User::operator==(const User &rhs) {
    return nickname == rhs.nickname;
}

// Destructor for the User class
User::~User() {}

// Returns the file descriptor of the user
int User::getFduser() const { return (fdUser); }

// Returns a reference to the user's nickname
std::string &User::getNickname() {return (nickname); }

// Returns the username (or an empty string if it's empty)
std::string User::getUser() { return (user.empty() ? std::string("") : user); }

// Returns the IP address of the user
std::string User::getIp() const { return (ip); }

// Returns the buffer containing data received from the user
std::string User::getBuffer() const { return (buff); }

// Returns the hostname in the format "nickname!user"
std::string User::getHostname() {
    return nickname + "!" + user;
}

// Returns the real name of the user
std::string User::getRealname() const {
    return (this->realname);
}

// Returns the user's modes as a string
std::string User::getUserModes() const {
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

// Returns true if the user is registered
bool User::isRegistered() const { return (registered); }

// Returns true if the user is connected
bool User::isConnected() { return (this->connected); }

// Returns true if the user is an operator
bool User::isOperator() const { return (this->isOp); }

// Checks if the user is invited to a specific channel
bool User::isInvited(std::string &channel) {
    for (size_t i = 0; i < this->invitation.size(); i++)
    {
        if (this->invitation[i] == channel)
            return (true);
    }
    return (false);
}

// Returns the state of a specific user mode option
bool User::getUserModeOption(size_t i) const { return (userModes[i].second); }

// Returns true if the user is the founder of the channel
bool User::isChannelFounder() const { return (chanFounder); }

// Returns the creation time of the user
time_t User::getCreationTime() const { return (created); }

// Sets the file descriptor of the user
void User::setFduser(int fd) { this->fdUser = fd; }

// Sets the user's nickname
void User::setNickname(const std::string &nickname) { this->nickname = nickname; }

// Sets the username
void User::setUser(std::string &user) { this->user = user; }

// Sets the user's IP address
void User::setIp(std::string ip) { this->ip = ip; }

// Appends data received to the user's buffer
void User::setBuffer(const std::string recv) { buff += recv; }

// Sets whether the user is registered
void User::setRegistered(bool val) { registered = val; }

// Sets whether the user is connected
void User::setConnected(bool val) { this->connected = val; }

// Sets whether the user is an operator
void User::setOperator(bool op) { this->isOp = op; }

// Sets whether the user is the founder of the channel
void User::setChannelFounder(bool founder) { this->chanFounder = founder; }


// Sets the user mode for a specific option
void User::setUserMode(size_t i, bool mode) { userModes[i].second = mode; }

// Sets the real name of the user
void User::setRealname(const std::string realname) { this->realname = realname; }

// Clears the user's buffer
void User::clearBuffer() { buff.clear(); }

// Adds an invitation for a specific channel
void User::addInvitation(const std::string& channel) {
    if (std::find(invitation.begin(), invitation.end(), channel) == invitation.end()) {
        invitation.push_back(channel);
    }
}

// // Retire l'invitation quand l'utilisateur rejoint le canal
// void User::removeInvitation(const std::string &channel) {
//     for (size_t i = 0; i < this->invitation.size(); i++)
//     {
//         if (this->invitation[i] == channel)
//         {
//             this->invitation.erase(this->invitation.begin() + i);
//             return;
//         }
//     }
// }

void User::removeInvitation(const std::string& channel) {
    std::vector<std::string>::iterator it = std::find(invitation.begin(), invitation.end(), channel);
    if (it != invitation.end()) {
        invitation.erase(it);
    }
}