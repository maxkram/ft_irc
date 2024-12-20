#ifndef SERVER_HPP
#define SERVER_HPP

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

#define RESET   "\033[0m"
#define RED     "\033[31m"
#define GREEN   "\033[32m"
#define RARROW  "\u2B9E "
#define LARROW  "\u2B9C "

class User;
class Channel;

class Server
{
private:
    int sock_fd;
    int  port;
    std::string	password;
    int	poll_nb;
    int	status;
    int	user_max;
    int sock_opt;
    int	addr_size;
    static bool signal;
    struct pollfd	new_user;
    std::vector<struct pollfd>	poll_fd;
    std::vector<User>	sock_user;
    std::vector<Channel>	channel;
    std::map<int, std::string>	buff;

public:
    Server();
    Server(Server const &obj);
    Server &operator=(Server const &obj);
    ~Server();

    int	getSockFd();
    int	getPort();
    int	getFdByNick(std::string nickname);
    std::string	getPassword();
    User	*getClientByFd(int fd);
    User	*getClientByNickname(std::string nickname);
    Channel	*getChannel(std::string name);

    void	setSockFd(int sock_fd);
    void	setPort(int port);
    void	setPassword(std::string password);
    void	setNewUser(User newuser);
    void	setNewChannel(Channel newchannel);
    void	setPollfd(pollfd fd);

    void	removeClient(int fd);
    void	removeFd(int fd);
    void	clearChannel(int fd);

    void	notifyUsers(std::string message, int fd);
    void	notifyClient2(int errnum, std::string user, std::string channel, int fd, std::string message);
    void	notifyClient3(int errnum, std::string user, int fd, std::string message);

    void	closeFd();
    bool	isPortValid(std::string port);
    bool	isRegistered(int fd);
    bool	isChannelAvailable(std::string channelName);

    void	initServer(int port, std::string pass);
    void	checkPollEvents();
    void	acceptNewClient();
    void	handleData(int fd);
    static void	handleSignal(int signum);

    int	splitMessage(std::string message, std::string split_mess[3]);
    int	splitParams(std::string params, std::string split_params[3]);
    std::vector<std::string>	extractParams(std::string &message);
    std::vector<std::string>	dissectMessage(std::string &message);
    std::vector<std::string>	splitBuffer(std::string buff);
    void	processCommand(std::string &command, int fd);

    void	PASS(std::string message, int fd);

    void	NICK(std::string message, int fd);
    bool	isNicknameUsed(std::string &nickname);
    bool	validNickname(std::string &nickname);
    void	updateNicknameChannel(std::string old, std::string n_nick);
    
    void	USER(std::string &message, int fd);
    
    void	    QUIT(std::string message, int fd);
    std::string	quitReason(std::string message);
    void	    quitReason2(std::string message, std::string str, std::string &reason);
    
    void	PING(std::string &message, int fd);
    
    void	JOIN(std::string message, int fd);
    int	    splitJoinParams(std::vector<std::pair<std::string, std::string> > &param, std::string message, int fd);
    void	addClientToExistChannel(std::vector<std::pair<std::string, std::string> > &param, int i , int j, int fd);
    void	createAndAddToNewChannel(std::vector<std::pair<std::string, std::string> >&param, int i, int fd);
    int	    channelUserCount(std::string user);
    bool	isUserInvited(User *user, std::string channel, int flag);
    
    void	INVITE(std::string message, int fd);

    void	    PART(std::string message, int fd);
    int	        splitPartParams(std::string message, std::vector<std::string> &param, std::string &reason, int fd);
    std::string	extractPartReason(std::string &message, std::vector<std::string> &param);
    void	    partReason(std::string message, std::string tofind, std::string &reason);
    
    void	    KICK(std::string message, int fd);
    std::string	splitKickParams(std::string message, std::vector<std::string> &param, std::string &user, int fd);
    std::string	kickReason(std::string &message, std::vector<std::string> &param);
    void	    kickReason2(std::string message, std::string tofind, std::string &comment);
    
    void	TOPIC(std::string message, int fd);
    
    void	                    MODE_CHANNEL(std::string &message, int fd);
    void	                    parseChannelMode(std::string message, std::string &channelname, std::string &modestring, std::string &param);
    std::vector<std::string>	splitChannelMode(std::string param);
    std::string	                updateMode(std::string ssmode, char addminus, char mode);
    std::string	                inviteOnly(Channel *channel, char addminus, std::string ssmode);
    std::string	                topicRestriction(Channel *channel, char addminus, std::string ssmode);
    std::string	                channelPassword(Channel *channel, char addminus, std::string ssmode, std::vector<std::string> paramsplit, std::string &arg, size_t &pos, int fd);
    bool	                    isValidChannelPassword(std::string password);
    std::string	                operatorPrivilege(Channel *channel, char addminus, std::string ssmode, std::vector<std::string> paramsplit, std::string &arg, size_t &pos, int fd);
    std::string                 setUserLimit(Channel *channel, char addminus, std::string ssmode, std::vector<std::string> paramsplit, std::string &arg, size_t &pos, int fd);
    bool	                    isValidUserLimit(std::string &limit);
    
    void            PRIVMSG(std::string &message, int fd);
    std::string     getChannelTarget(std::string &target);
    std::string	    getUserTarget(int fd);
    int	        validatePrivmsgSyntax(std::string split_mess[3], std::string split_params[3]);
    int	        handlePrivmsg(std::string split_message[3], std::string split_params[3], int fd);
};

#endif