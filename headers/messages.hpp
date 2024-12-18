#pragma once

# define UNDER	std::string("\e[4m")
# define RESET	std::string("\e[0m")
# define RED	std::string("\e[35m")
# define BLUE	std::string("\e[95m")
# define CYAN   std::string("\e[38;5;208m")
# define GREEN	std::string("\e[38;5;82m")
# define YELLOW	std::string("\e[38;5;220m")

# define CLIENT client.getNick() + "!" + client.getUsername() + "@" + client.getIP()
# define TERMINATOR terminator.getNick() + "!" + terminator.getUsername() + "@" + terminator.getIP()

# define RPL_WELCOME GREEN + ":Welcome to the vibrant and colorful ft_IRC Network " + CLIENT + "! Enjoy your stay!" + RESET
# define RPL_NOTOPIC YELLOW + ":This channel currently has no topic set. Start a discussion!" + RESET
# define RPL_ENDOFWHOIS GREEN + ":End of WHOIS query for " + CLIENT + RESET
# define RPL_YOUREOPER GREEN + ":Congratulations, you are now an IRC operator. Handle the power responsibly!" + RESET
# define RPL_UNAWAY GREEN + ":You are back online and no longer marked as away." + RESET
# define RPL_NOWAWAY GREEN + ":You are marked as away. Let others know why!" + RESET
# define RPL_ENDOFWHO GREEN + ":End of WHO query. Hope you found what you were looking for!" + RESET
# define RPL_WHOISIDLE YELLOW + " :User has been idle for seconds. Reach out or wait!" + RESET

# define ERR_NOTREGISTERED RED + ":You have not registered. Use /REGISTER to join the fun!" + RESET
# define ERR_NOSUCHSERVICE RED + ":No such service exists on this network. Check your command!" + RESET
# define ERR_NOPRIVILEGES RED + ":Permission Denied - You're not an IRC operator." + RESET
# define ERR_NOSUCHNICK RED + ":No such nick/channel. Please check the name and try again." + RESET
# define ERR_NORECIPIENT YELLOW + ":No recipient given (PRIVMSG). Specify a user or channel." + RESET
# define ERR_NOTEXTTOSEND YELLOW + ":No text provided to send. Say something!" + RESET
# define ERR_CANNOTSENDTOCHAN RED + ":Message could not be sent to the channel." + RESET
# define ERR_CHANOPRIVSNEEDED RED + ":You need to be a channel operator to do that!" + RESET
# define ERR_USERNOTINCHANNEL RED + ":The user isn't on that channel." + RESET
# define ERR_NEEDMOREPARAMS YELLOW + ":Not enough parameters provided. Check the command usage." + RESET
# define ERR_ALREADYREGISTRED RED + ":You are already registered. Command unauthorized!" + RESET
# define ERR_PASSWDMISMATCH RED + ":Incorrect password. Try again or reset it." + RESET
# define ERR_NOOPERHOST RED + ":No O-lines for your host." + RESET
# define ERR_NOSUCHCHANNEL RED + ":No such channel. Check the name and try again." + RESET
# define ERR_NOTONCHANNEL RED + " :You're not on that channel. Join it first!" + RESET
# define ERR_USERONCHANNEL YELLOW + ":The user is already on the channel." + RESET
# define ERR_INVITEONLYCHAN RED + " :Cannot join channel (+i). You need an invitation." + RESET
# define ERR_BADCHANNELKEY RED + " :Cannot join channel (+k). Incorrect key provided." + RESET
# define ERR_UNKNOWNCOMMAND YELLOW + ":Unknown command. Check the syntax or available commands." + RESET
# define ERR_NONICKNAMEGIVEN YELLOW + ":No nickname given. Please provide one." + RESET
# define ERR_NICKNAMEINUSE RED + ":The nickname is already in use. Choose another one!" + RESET
# define ERR_ERRONEUSNICKNAME YELLOW + ":Invalid nickname. Please follow the IRC nickname rules." + RESET
# define ERR_UMODEUNKNOWNFLAG YELLOW + ":Unknown user mode flag. Check the available options." + RESET
# define ERR_USERSDONTMATCH YELLOW + ":Cannot change modes for other users." + RESET
# define ERR_NOORIGIN RED + ":No origin specified. Provide more details." + RESET
# define ERR_NOSUCHSERVER RED + ":No such server. Check the address and try again." + RESET
# define ERR_UNKNOWNMODE YELLOW + ":Unknown mode character provided. Check the syntax." + RESET
