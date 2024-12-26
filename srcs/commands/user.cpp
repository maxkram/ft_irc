#include "../../includes/server.hpp"

void	Server::USER(std::string &message, int fd)
{
	User	*user;
	std::vector<std::string>	param;

	user = getClientByFd(fd);
	param = extractParams(message);
	if (user && param.size() < 5)
	{
		notifyUsers(ERR_NOTENOUGHPARAMETERS(user->getNickname()), fd);
		return;
	}
	if (!user || !user->isRegistered())
	{
		notifyUsers(ERR_NOTREGISTERED(std::string("*")), fd);
	}
	else if (user && !user->getUser().empty())
	{
		notifyUsers(ERR_ALREADYREGISTERED(user->getNickname()), fd);
		return;
	}
	else
	{
		user->setUser(param[1]);
		std::string	realname = param[4];
		realname = realname.substr(1);
		user->setRealname(realname);
	}

	if (user && user->isRegistered() && !user->getUser().empty() && !user->getNickname().empty() && user->getNickname() != "*" && !user->isConnected())
	{
		user->setConnected(true);
		notifyUsers(RPL_WELCOME(user->getNickname()), fd);
	}
}
