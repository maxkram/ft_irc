#include "../headers/Command.hpp"

Command::Command(std::string const & password, std::list<Client> & clients, std::string const & IP) : password(password), clients(clients), IP(IP) 
{
	cmdMap[std::string("PRIVMSG")] = &Command::privateMessage;
	cmdMap[std::string("AWAY")] = &Command::away;
	cmdMap[std::string("NOTICE")] = &Command::notice;
	cmdMap[std::string("INVITE")] = &Command::invite;
	cmdMap[std::string("TOPIC")] = &Command::topic;
	cmdMap[std::string("USER")] = &Command::user; 
	cmdMap[std::string("PASS")] = &Command::pass;
	cmdMap[std::string("QUIT")] = &Command::quit;
	cmdMap[std::string("OPER")] = &Command::oper;
	cmdMap[std::string("MODE")] = &Command::mode;
	cmdMap[std::string("JOIN")] = &Command::join;
	cmdMap[std::string("PART")] = &Command::part;
	cmdMap[std::string("NAMES")] = &Command::names;
	cmdMap[std::string("LIST")] = &Command::list;
	cmdMap[std::string("KICK")] = &Command::kick;
	cmdMap[std::string("WHOIS")] = &Command::whois;
	cmdMap[std::string("WHO")] = &Command::who;
	cmdMap[std::string("NICK")] = &Command::nick;
	cmdMap[std::string("PING")] = &Command::ping;
	cmdMap[std::string("KILL")] = &Command::kill;
}

Command::~Command() {}

void Command::sendAll(const Client& client, const std::string& prefix, const std::string& cmd, const std::string& args) {
    std::string message = ":" + prefix + " " + cmd + " " + args + "\r\n";
    send(client.getSocket(), message.c_str(), message.length(), 0);
}

void	Command::sendMessage(Client const & client, std::string nb, std::string opt, std::string msg)
{
	std::string	message(":" + IP);
	if (!opt.empty())
		message += " " + nb + " " + client.getNick() + " " + opt + " " + msg + "\r\n";
	else
		message += " " + nb + " " + client.getNick() + " " + msg + "\r\n";
	send(client.getSocket(), message.c_str(), message.length(), 0);
}

void    Command::sendConfirm(Client const & client, std::string const & cmd, std::string const & opt)
{
	std::string message(":" + CLIENT);
	if (opt.empty())
		message += " " + cmd + "\r\n";
	else
		message += " " + cmd + " :" + opt + "\r\n";
	send(client.getSocket(), message.c_str(), message.length(), 0);
}

void    Command::sendConfirmTo(Client const & client, Client const & terminator, std::string const & cmd, std::string const & opt)
{
	std::string message(":" + TERMINATOR);
	if (opt.empty())
		message += " " + cmd + "\r\n";
	else
		message += " " + cmd + " :" + opt + "\r\n";
	send(client.getSocket(), message.c_str(), message.length(), 0);
}

void    Command::sendError(Client const & client, std::string const & arg, std::string const & opt)
{
	std::string message("ERROR");
	message += " :" + arg + ": " + opt + "\r\n";
	send(client.getSocket(), message.c_str(), message.length(), 0);
}

void    Command::closeLink(Client const & client, std::string const & arg, std::string const & opt)
{
	std::string message;
	message = arg + ": " + opt + "\r\n";
	send(client.getSocket(), message.c_str(), message.length(), 0);
}

void    Command::sendSpecConfirm(Client const & client, std::string const & prefix, std::string const & cmd, std::string const & opt)
{
	std::string message(":" + prefix);
	message += " " + cmd + " " + prefix + " :" + opt + "\r\n";
	send(client.getSocket(), message.c_str(), message.length(), 0);
}

bool	Command::isSpecialChar(char c) const
{
	if (c == '[' || c == ']' || c ==  '\\' || c == '`' || c ==  '_'
			|| c ==  '^' || c ==  '{' || c ==  '|' || c ==  '}')
		return (true);
	return (false);
}

void	Command::setIP(std::string const & val) { IP = val;}

std::vector<std::string> Command::splitAll(const std::string& str, const std::string& delimiter) {
    std::vector<std::string> tokens;
    std::string::size_type start = 0, end = 0;

    while ((end = str.find(delimiter, start)) != std::string::npos) {
        if (end != start) // Skip empty tokens
            tokens.push_back(str.substr(start, end - start));
        start = end + delimiter.size();
    }

    if (start < str.size()) // Add last token
        tokens.push_back(str.substr(start));

    return tokens;
}


std::vector<std::string>    Command::splitString(std::string const & s, std::string const & seperator)
{
	std::vector<std::string> output;
	std::string::size_type prev_pos = 0, pos = 0;
	while((pos = s.find(seperator, pos)) != std::string::npos)
	{
		std::string substring( s.substr(prev_pos, pos-prev_pos) );
		if (!substring.empty())
			output.push_back(substring);
		pos += seperator.length();
		prev_pos = pos;
	}
	std::string substring( s.substr(prev_pos, pos-prev_pos) );
	if (!substring.empty())
		output.push_back(substring);
	return output;
}

std::vector<std::string>	Command::splitCommand(std::string const & s, std::string const & seperator)
{
	 	std::vector<std::string> output;
		std::string::size_type prev_pos = 0, pos = 0;
		size_t i = 0;
		bool	point;
		for (; i < s.length(); i++)
			if (s[i] != ' ')
				break;
		if (s[i] == ':')
			point = false;
		else
			point = true;
			
 		while((pos = s.find(seperator, pos)) != std::string::npos)
 		{
	 		if (s[prev_pos] == ':')
 			{
				if (point)
				{
	 				std::string substring( s.substr(prev_pos) );
 					output.push_back(substring);
 					return (output);
				}
				else
					point = true;
 			}
 			std::string substring( s.substr(prev_pos, pos-prev_pos) );
 			if (!substring.empty())
 				output.push_back(substring);
 			pos += seperator.length();
 			prev_pos = pos;
 		}
 		std::string substring( s.substr(prev_pos, pos-prev_pos) );
 		if (!substring.empty())
	 		output.push_back(substring);
	 return output;
}

void	Command::parseCommand(Client & client)
{
	std::vector<std::string>	cmd;
	if(client.getBuffer()[client.getBuffer().length() - 2] == '\r')
		cmd = splitString(client.getBuffer(), "\r\n");
	else
		cmd = splitString(client.getBuffer(), "\n");
	std::vector<std::string>::iterator	it = cmd.begin();
	std::map<std::string, pfunc>::iterator	iter;
	for (; it != cmd.end(); it++)
	{
		std::vector<std::string>	cmds = splitCommand(*it, " ");
		if (cmds[0][0] == ':')
		{
			cmds[0].erase(0, 1);
			if (cmds[0] != client.getNick())
				return ;
			cmds.erase(cmds.begin());
		}
		std::string	tmp(cmds[0]);
		std::transform(cmds[0].begin(), cmds[0].end(),cmds[0].begin(), toupper);
		iter = cmdMap.find(cmds[0]);
		if (iter != cmdMap.end())
			CALL_MEMBER_FN(*this, iter->second) (cmds, client);
		else if (client.isRegistered())
			sendMessage(client, "421", tmp, ERR_UNKNOWNCOMMAND);
	}
}

void	Command::registerClient(Client & client)
{
	if (client.isNicked() && client.isUsered())
	{
		if (!password.empty() && client.getPass() != password)
		{
			sendMessage(client, "464", "", ERR_PASSWDMISMATCH);
			return ;
		}
		client.setRegistered(true);
		sendMessage(client, "001", "", RPL_WELCOME); 
	}
}

// std::string Command::usedMods(std::string channel)
// {
// 	std::map<std::string, Channel>::iterator    itMap = chanMap.begin();    
// 	std::string usedMod("+");
// 	for(; itMap != chanMap.end(); itMap++)
// 	{
// 		if (itMap->first == channel)
// 		{
// 			if (itMap->second.isTopicEnabled())
// 				usedMod.append("t");                        
// 			if (itMap->second.isInviteEnabled())
// 				usedMod.append("i");                        
// 			if (itMap->second.hasPassword())
// 				usedMod.append("k");                        
// 			break;
// 		}
// 	}
// 	return usedMod;
// }

std::string Command::getUsedModes(const std::string& channel) {
    std::map<std::string, Channel>::iterator it = chanMap.find(channel);
    if (it == chanMap.end())
        return "+";

    std::string modes("+");
    if (it->second.isTopicEnabled())
        modes += "t";
    if (it->second.isInviteEnabled())
        modes += "i";
    if (it->second.hasPassword())
        modes += "k";

    return modes;
}
