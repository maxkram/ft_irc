#ifndef USER_HPP
#define USER_HPP

#include "server.hpp"
#include "channel.hpp"

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
    std::vector<std::pair<char, bool> > userModes;

public:
    User();
    User(int fd, std::string nickname, std::string user);
    User(User const &obj);
    User &operator=(User const &obj);
    bool operator==(User const &rhs);
    ~User();

    int                 getFduser();
    std::string         &getNickname();
    std::string         getUser();
    std::string         getIp();
    std::string         getBuffer();
    std::string         getHostname();
    std::string         getUserModes();
    std::string         getRealname() const;
    bool                isRegistered();
    bool                isConnected();
    bool                isInvited(std::string &channel);
    bool                isOperator();
    bool                getUserModeOption(size_t i);
    bool                isChannelFounder();
    time_t              getCreationTime() const;

    void setFduser(int fd);
    void setNickname(std::string &nickname);
    void setUser(std::string &user);
    void setIp(std::string ip);
    void setBuffer(std::string recv);
    void setRegistered(bool val);
    void setConnected(bool val);
    void setOperator(bool op);
    void setChannelFounder(bool founder);
    void setUserMode(size_t i, bool mode);
    void setRealname(std::string realname);

    void clearBuffer();
    void addInvitation(std::string &channel);
    void removeInvitation(std::string &channel);
};

#endif