#ifndef CHANNEL_HPP
#define CHANNEL_HPP

#include "server.hpp"
#include "user.hpp"

class User;

class Channel
{

private:
    std::string channelName;
    std::string topicName;
    std::string password;
    std::string created_at;
    int topic;
    int key;
    int limit;
    int invitOnly;
    bool topicRestriction;

    std::vector<User> sock_user;
    std::vector<User> admin;
    std::vector<std::pair<char, bool> > channelMode;

public:
    Channel();
    Channel(Channel const &src);
    Channel &operator=(Channel const &rhs);
    ~Channel();

    std::string getChannelName();
    std::string getTopicName();
    std::string getChannelPassword();
    std::string getUserList();
    std::string getCreationDate();
    std::string getChannelModes();
    std::vector<User> &getUsers();
    std::vector<User> &getOperators();
    User *getUserByFd(int fd);
    User *getOperatorByFd(int fd);
    User *getFindUserByName(std::string name);
    int getTopicStatus();
    int getKeyStatus();
    int getUserLimit();
    int getInviteOnlyStatus();
    bool getTopicRestriction() const;
    bool getChannelModeOption(size_t i);
    std::vector<User *> getUserPointers();

    void setChannelName(std::string name);
    void setTopicName(std::string topic);
    void setChannelPassword(std::string password);
    void setTopicStatus(int topic);
    void setKeyStatus(int key);
    void setUserLimit(int limit);
    void setInviteOnlyStatus(int invitOnly);
    void setTopicRestriction(bool restriction);
    void setChannelMode(size_t i, bool mode);
    void setCreationDate();

    void removeUserByFd(int fd);
    void removeOperatorByFd(int fd);

    void broadcastMessage(std::string reply);
    void broadcastMessage2(std::string reply, int fd);

    size_t getUserCount();
    bool isUserInChannel(std::string &name);
    bool isUserOperator(std::string &name);
    void checkChannelName(std::string channelName);
    void addOperatorOnChannel(User user);
    void addUser(User user);
    bool promoteUserToOperator(std::string &name);
    void promoteFirstUserToOperator();
    bool demoteOperatorToUser(std::string &name);
    void notifyUsers(std::string msg, User &author);
    bool hasOperators();

};

#endif
