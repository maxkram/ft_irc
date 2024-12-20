#include "../../includes/server.hpp"

// Cette fonction gère la commande USER, qui permet de définir les informations utilisateur pour un client IRC
void	Server::USER(std::string &message, int fd)
{
	User *user = getClientByFd(fd);
	// Extract parameters from the message
	std::vector<std::string> param = extractParams(message);

	// Check if there are enough parameters
	if (user && param.size() < 5) {
		notifyUsers(ERR_NOTENOUGHPARAMETERS(user->getNickname()), fd);
		return;
	}

	if (!user || !user->isRegistered()) {
		notifyUsers(ERR_NOTREGISTERED(std::string("*")), fd);
	}

	// Check if the user is already registered
	else if (user && !user->getUser().empty()) {
		notifyUsers(ERR_ALREADYREGISTERED(user->getNickname()), fd);
		return;
	} else 	{
		// Set the username and realname
		user->setUser(param[1]);
		// Ensure realname starts without a colon (if present)
		std::string	realname = param[4];
		realname = realname.substr(1);
		user->setRealname(realname);
	}

	// Final registration check and welcome notification
	if (user && user->isRegistered()
		&& !user->getUser().empty()
		&& !user->getNickname().empty()
		&& user->getNickname() != "*"
		&& !user->isConnected()) {
		user->setConnected(true);
		notifyUsers(RPL_WELCOME(user->getNickname()), fd);
	}
}
