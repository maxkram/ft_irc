#pragma once

#include <iostream>
#include <string>
#include <cstring>
#include <cstdlib>
#include <cerrno>
#include <ctime>
#include <stdio.h>
#include <sstream>
#include <algorithm>
#include <map>
#include <vector>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <fcntl.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <poll.h>
#include <csignal>
#include "user.hpp"
#include "channel.hpp"
#include "replies.hpp"
#include "colors.hpp"
// #define RESET   "\033[0m"
// #define RED     "\033[38;5;196m"   // Bright red
// #define GREEN   "\033[38;5;46m"    // Bright green
// #define BLUE    "\033[38;5;27m"    // Bright blue
// #define PURPLE  "\033[38;5;129m"   // Exotic purple
// #define ORANGE  "\033[38;5;208m"   // Exotic orange
// #define CYAN    "\033[38;5;51m"    // Exotic cyan
// #define RARROW  "\u2B9E "          // Right arrow
// #define LARROW  "\u2B9C "          // Left arrow


class User;
class Channel;

class Server
{
private:
    int sockFd;
    int  port;
    std::string	password;
    int	pollNb;
    int	status;
    int	userMax;
    int sockOpt;
    int	addr_size;
    static bool signal;
    struct pollfd	new_user;
    std::vector<struct pollfd>	pollFds;
    std::vector<User>	sock_user;
    std::vector<Channel>	channel;
    std::map<int, std::string>	buff;

    void logMessage(const std::string& message, const std::string& color = RESET) const;

public:
    Server();
    Server(const Server &obj);
    Server &operator=(const Server &obj);
    ~Server();

    // Getters
    int	getSockFd() const;
    int	getPort() const;
    int	getFdByNick(std::string nickname);
    const std::string	getPassword() const;
    User	*getClientByFd(int fd);
    User	*getUserByNickname(const std::string nickname);
    Channel	*getChannel(const std::string name);

    // Setters
    void	setSocketFd(int fd);
    void	setPort(int port);
    void	setPassword(const std::string password);
    void	setNewUser(User newuser);
    void	setNewChannel(Channel newchannel);
    void	setPollfd(pollfd fd);

    // User and Channel Management
    void	removeUserByFd(int fd);
    void	removePollFd(int fd);
    void	clearEmptyChannels(int fd);

    // Messaging
    void	notifyUsers(std::string message, int fd);
    void	notifyClient2(int errnum, std::string user, std::string channel, int fd, std::string message);
    void	notifyClient3(int errnum, std::string user, int fd, std::string message);
    void    notifyClient(int errorCode, const std::string& user, const std::string& additionalInfo, int fd, const std::string& message);
    
    // Validation
    void	closeFd();
    bool	isPortValid(const std::string& port) const;
    bool	isRegistered(int fd);
    bool	isChannelAvailable(std::string channelName);

    // Server Initialization
    void	initialize(int port, std::string pass);
    void	checkPollEvents();
    void	acceptNewClient();
    void	handleData(int fd);
    static void	handleSignal(int signum);

    int	splitMessage(std::string message, std::string split_mess[3]);
    int	splitParams(std::string params, std::string split_params[3]);
    std::vector<std::string>	extractParams(std::string &message);
    std::vector<std::string>	dissectMessage(std::string &message);
    std::vector<std::string>	splitBuffer(std::string buff);

    // Command Handling
    void	processCommand(std::string &command, int fd);

    void	PASS(std::string message, int fd);

    void	NICK(std::string message, int fd);
    bool	isNicknameUsed(const std::string &nickname);
    // bool	validNickname(std::string &nickname);
    bool    validNickname(std::string& nickname);
    // void	updateNicknameChannel(std::string old, std::string n_nick);
    void    updateNicknameChannel(std::string& oldNickname, const std::string& newNickname);
    bool    updateNicknameInList(std::vector<User>& users, const std::string& oldNickname, const std::string& newNickname);

    void	USER(std::string &message, int fd);
    
    void	    QUIT(std::string message, int fd);
    std::string quitReason(const std::string &message);
    // std::string	quitReason(std::string message);
    // void	    quitReason2(std::string message, std::string str, std::string &reason);
    
    void	PING(std::string &message, int fd);
    
    void	JOIN(std::string message, int fd);
    int	    splitJoinParams(std::vector<std::pair<std::string, std::string> > &param, std::string message, int fd);
    void	addClientToExistChannel(std::vector<std::pair<std::string, std::string> > &param, int i , int j, int fd);
    void	createAndAddToNewChannel(std::vector<std::pair<std::string, std::string> >&param, int i, int fd);
    // void    createAndAddToNewChannel(const std::string& channelName, const std::string& key, int fd);
    int	    channelUserCount(std::string user);
    bool	isUserInvited(User *user, std::string channel, int flag);
    
    void	INVITE(std::string message, int fd);

    void	    PART(std::string message, int fd);
    // int	        splitPartParams(std::string message, std::vector<std::string> &param, std::string &reason, int fd);
    int         splitPartParams(const std::string& message, std::vector<std::string>& channels, std::string& reason, int fd);
    // std::string	extractPartReason(std::string &message, std::vector<std::string> &param);
    // void	    partReason(std::string message, std::string tofind, std::string &reason);
    void        partReason(const std::string& message, const std::string& tofind, std::string& reason);

    void	    KICK(std::string message, int fd);
    std::string	splitKickParams(std::string message, std::vector<std::string> &param, std::string &user, int fd);
    std::string	kickReason(std::string &message, std::vector<std::string> &param);
    void	    kickReason2(std::string message, std::string tofind, std::string &comment);
    
    void	TOPIC(std::string message, int fd);
    
    void	                    MODE_CHANNEL(std::string &message, int fd);
    // void	                    parseChannelMode(std::string message, std::string &channelname, std::string &modestring, std::string &param);
    void	                    parseChannelMode(const std::string &message, std::string &channelname, std::string &modestring, std::string &param);
    std::vector<std::string>	splitChannelMode(std::string param);
    std::string	                updateMode(std::string ssmode, char addminus, char mode);
    std::string	                inviteOnly(Channel *channel, char addminus, std::string ssmode);
    std::string	                topicRestriction(Channel *channel, char addminus, std::string ssmode);
    std::string	                channelPassword(Channel *channel, char addminus, std::string ssmode, std::vector<std::string> paramsplit, std::string &arg, size_t &pos, int fd);
    bool	                    isValidChannelPassword(const std::string &password);
    std::string	                operatorPrivilege(Channel *channel, char addminus, std::string ssmode, std::vector<std::string> paramsplit, std::string &arg, size_t &pos, int fd);
    std::string                 setUserLimit(Channel *channel, char addminus, std::string ssmode, std::vector<std::string> paramsplit, std::string &arg, size_t &pos, int fd);
    bool	                    isValidUserLimit(const std::string &limit);
    
    void            PRIVMSG(std::string &message, int fd);
    std::string     getChannelTarget(std::string &target);
    std::string	    getUserTarget(int fd);
    int	        validatePrivmsgSyntax(std::string split_mess[3], std::string split_params[3]);
    int	        handlePrivmsg(std::string split_message[3], std::string split_params[3], int fd);
    // std::string     parseNickname(const std::string& message);
    std::vector<std::string>    splitByDelimiter(const std::string &input, char delimiter);
    void        sendJoinNotifications(Channel* chan, int fd);
    // void        handleInvite(const std::string& invitedUserName, const std::string& channelName, User* sender, Channel* channel);
    // std::vector<std::string>    extractParams(const std::string& message);
};
