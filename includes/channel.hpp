#pragma once

#include "server.hpp"
#include <string>
#include <vector>
#include <ctime>
#include <iostream>
#include <stdexcept>
#include "user.hpp"
#include "colors.hpp"
class User;

class Channel {

private:
    std::string channelName;
    std::string topicName;
    std::string password;
    std::string created_at;
    bool topic; // Whether a topic is set
    bool key; // Whether a key is required
    size_t userLimit; // Maximum number of users
    bool inviteOnly; // Whether the channel is invite-only
    bool topicRestriction;

    std::vector<User> sock_user;
    std::vector<User> admin;
    std::vector<std::pair<char, bool> > channelModes;

public:
    Channel();
    Channel(const Channel &src);
    Channel &operator=(const Channel &rhs);
    ~Channel();
    // Getters
    std::string getChannelName() const;
    std::string getTopicName() const;
    std::string getChannelPassword() const;
    std::string getUserList();
    std::string getCreationDate() const;
    std::string getChannelModes() const;

    std::vector<User> &getUsers();
    std::vector<User> &getOperators();

    User *getUserByFd(int fd);
    User *getOperatorByFd(int fd);
    User *getFindUserByName(const std::string name);

    int getTopicStatus() const;
    int getKeyStatus() const;
    int getUserLimit() const;
    int getInviteOnlyStatus();
    bool getTopicRestriction() const;
    bool getChannelModeOption(size_t i) const;
    std::vector<User *> getUserPointers();

    // Setters
    void setChannelName(std::string name);
    void setTopicName(const std::string topic);
    void setChannelPassword(const std::string password);
    void setTopicStatus(int topic);
    void setKeyStatus(int key);
    void setUserLimit(int limit);
    void setInviteOnlyStatus(int invitOnly);
    void setTopicRestriction(bool restriction);
    void setChannelMode(size_t i, bool mode);
    void setCreationDate();

    // User and Operator Management
    void removeUserByFd(int fd);
    void removeOperatorByFd(int fd);
    void addUser(const User user);
    void addOperatorOnChannel(const User user);

    bool promoteUserToOperator(const std::string &name);
    void promoteFirstUserToOperator();
    bool demoteOperatorToUser(const std::string &name);
    
    // Messaging
    void broadcastMessage(std::string reply);
    void broadcastMessage2(std::string reply, int fd);

    // Utility
    size_t getUserCount();
    bool isUserInChannel(std::string &name);
    bool isUserOperator(std::string &name);
    void notifyUsers(std::string msg, User &author);
    bool hasOperators();

    // Validation
    void checkChannelName(std::string channelName);
};

