#pragma once

#include "server.hpp"
#include "user.hpp"

class User;

class Channel
{

private:
    std::string channelName;
    std::string topicName;
    std::string password;
    std::string creationTime;
    int topic;
    int key;
    int limit;
    int invitOnly;
    bool restrictTopic;

    std::vector<User> regularUsers;
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
    int isTopicSet();
    int hasPassword();
    int getUserLimit();
    int isInviteOnly();
    bool isTopicRestricted() const;
    bool isModeEnabled(size_t i);
    std::vector<User *> getUserPointers();

    void setChannelName(std::string name);
    void setTopic(std::string topic);
    void setPassword(std::string password);
    void setTopicFlag(int topic);
    void setKeyStatus(int key);
    void setUserLimit(int limit);
    void setInviteOnly(int invitOnly);
    void setTopicRestriction(bool restriction);
    void setChannelMode(size_t i, bool mode);
    void setCreationDate();

    void removeUserByFd(int fd);
    void removeOperatorByFd(int fd);

    void broadcastMessage(std::string reply);
    void sendMessageToAllExcept(std::string reply, int fd);

    size_t getUserCount();
    bool isUserInChannel(std::string &name);
    bool isUserOperator(std::string &name);
    void validateName(std::string channelName);
    void addOperator(User user);
    void addRegularUser(User user);
    bool promoteToOperator(std::string &name);
    void promoteFirstUser();
    bool demoteOperatorToUser(std::string &name);
    void notifyUsers(std::string msg, User &author);
    bool hasOperators();
};
