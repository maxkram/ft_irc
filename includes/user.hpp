#pragma once

#include "server.hpp"
#include "channel.hpp"
#include "colors.hpp"
#include <string>
#include <vector>
#include <ctime>
#include <algorithm>

class Channel;

class User
{
private:
    int                                 fdUser;
    std::string                         nickname;
    std::string                         user;
    std::string                         ip;
    std::string                         buff;
    std::string                         realname;
    bool                                registered;
    bool                                connected;
    bool                                isOp;
    bool                                chanFounder;
    time_t                              created;
    std::vector<std::string>            invitation;
    std::vector<std::pair<char, bool> > userModes;// User mode flags (e.g., 'i', 'o')

public:
    User();
    User(int fd, const std::string& nickname, const std::string& user);
    User(const User &obj);
    User &operator=(const User &obj);
    bool operator==(const User &rhs);
    ~User();

    // Getters
    int                 getFduser() const;
    std::string         &getNickname();
    std::string         getUser();
    std::string         getIp() const;
    std::string         getBuffer() const;
    std::string         getHostname();
    std::string         getUserModes() const;
    std::string         getRealname() const;
    bool                isRegistered() const;
    bool                isConnected();
    bool                isInvited(std::string &channel);
    bool                isOperator() const;
    bool                getUserModeOption(size_t i) const;
    bool                isChannelFounder() const;
    time_t              getCreationTime() const;
    // Setters
    void setFduser(int fd);
    void setNickname(const std::string &nickname);
    void setUser(std::string &user);
    void setIp(std::string ip);
    void setBuffer(const std::string recv);
    void setRegistered(bool val);
    void setConnected(bool val);
    void setOperator(bool op);
    void setChannelFounder(bool founder);
    void setUserMode(size_t i, bool mode);
    void setRealname(const std::string realname);

    // Utilities
    void clearBuffer();
    void addInvitation(const std::string &channel);
    void removeInvitation(const std::string &channel);
};
