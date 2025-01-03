#pragma once

// #define ERR_NOTENOUGHPARAMETERS(client) (": 461 " + client + " :Not enough parameters\r\n")
#define ERR_MISSING_PARAMETERS(client)        (": 461 " + client + " :Not enough parameters\r\n")
#define ERR_PASSWORD_INCORRECT(client) (": 464 " + client + " :Password incorrect\r\n")
#define ERR_ALREADY_REGISTERED(client) (": 462 " + client + " :You may not reregister\r\n")
#define ERR_USER_NICKNAME_IN_USE(client, nick) (": 433 " + client + " " + nick + " :Nickname is already in use\r\n")
#define ERR_INVALID_NICKNAME(client) (": 432 " + client + " :Erroneus nickname\r\n")
#define ERR_USER_NOT_REGISTERED(client) (": 451 " + client + " :You have not registered!\r\n")
#define ERR_CHANNEL_NOT_FOUND(client, channel) (": 403 " + client + " " + channel + " :No such channel\r\n")
#define ERR_CHANNEL_NOT_OPERATOR(client, channel) (": 482 " + client + " #" + channel + " :You're not a channel operator\r\n")
#define ERR_UNKNOWN_MODE(nickname, channel, mode) ": 472 " + nickname + " #" + channel + " " + mode + " :is not a recognised channel mode\r\n"
#define ERR_NO_SUCH_NICK_CHANNEL(channel, nickname) (": 401 #" + channel + " " + nickname + " :No such nick/channel\r\n")
#define ERR_USER_NOT_FOUND(nickname) (": 401 " + nickname + " :No such nick/channel\r\n")
#define ERR_NEEDMODEPARAM(channel, mode) (": 696 #" + channel + " * You must specify a parameter for the key mode. " + mode + "\r\n")
#define ERR_INVALIDMODEPARAM(channel, mode) ": 696 #" + channel + " Invalid mode parameter. " + mode + "\r\n"
#define ERR_KEYEXIST(channel) ": 467 #" + channel + " Channel key already set. " + "\r\n"
#define ERR_USERONCHANNEL(client, nickname, channel) (": 443 " + client + " " + nickname + " #" + channel + " :is already on channel" + "\r\n")
#define ERR_MESSAGE_NO_RECIPIENT(command) (": 411 No recipient given (" + command + ")")
#define ERR_MESSAGE_NO_TEXT() (": 412 No text to send\r\n")
#define ERR_NOTOPLEVEL(mask) (": 413 " + mask + " No toplevel domain specified\r\n")
#define ERR_WILDCARDTOPLEVEL(mask) (": 414 " + mask + " Wildcard in toplevel domain\r\n")
#define ERR_TOOMANYRECIPIENTS(target) (": 407 " + target + " Duplicate recipients. No message delivered\r\n")
#define ERR_UNKNOWNCOMMAND(client, command) (": 421 " + client + " " + command + " :Unknown command\r\n")

#define RPL_PONG "PONG ircserv localhost\r\n"
#define RPL_SERVER_WELCOME(client) (":localhost 001 " + client + " : Welcome to the ft_irc Network, " + client + "\r\n")
#define RPL_USER_NICK_CHANGE(old, client) (":" + old + " NICK " + client + "\r\n")
#define RPL_CHANNEL_JOIN(hostname, ipaddress, channel) (":" + hostname + "@" + ipaddress + " JOIN #" + channel + "\r\n")
#define RPL_CHANNEL_NAME_REPLY(client, channel, clientslist) (": 353 " + client + " @ #" + channel + " :" + clientslist + "\r\n")
#define RPL_CHANNEL_NAME_END(client, channel) (": 366 " + client + " #" + channel + " :END of /NAMES list\r\n")
#define RPL_TOPICWHOTIME(client, channel, nick, setat) (": 333 " + client + " #" + channel + " " + nick + " " + setat + "\r\n")
#define RPL_TOPIC(client, channel, topic) (": 332 " + client + " #" + channel + " :" + topic + "\r\n")
#define RPL_NOTOPICSET(channel) (": 331 " + channel + " :No topic is set.\r\n")
#define RPL_YOUAREOPERATOR(client) (": 381 " + client + " :You are now an IRC operator\r\n")
#define RPL_CHANNELMODE(client, channel, mode) ": 324 " + client + " #" + channel + " " + mode + "\r\n"
#define RPL_CREATIONTIME(client, channel, createdat) ": 329 " + client + " #" + channel + " " + createdat + "\r\n"
#define RPL_CHANGEMODE(hostname, channel, mode, arguments) (":" + hostname + " MODE #" + channel + " " + mode + " " + arguments + "\r\n")
#define RPL_INVITE(client, nickname, channel) (": 341 " + client + " invite " + nickname + " to #" + channel + "\r\n")
#define RPL_PRIVMSGCHANNEL(hostname, ipaddress, receiver, message) (":" + hostname + "@" + ipaddress + " PRIVMSG " + "#" + receiver + " " + message + "\r\n")
#define RPL_PRIVMSGUSER(hostname, ipaddress, receiver, message) (":" + hostname + "@" + ipaddress + " PRIVMSG " + receiver + " " + message + "\r\n")
