#include "../../includes/server.hpp"

void	Server::USER(std::string &message, int fd)
{
	User	*user = getClientByFd(fd);
	std::vector<std::string>	param = extractParams(message);

	if (user && param.size() < 5)
	{
		notifyUsers(ERR_MISSING_PARAMETERS(user->getNickname()), fd);
		return;
	}
	if (!user || !user->isRegistered())
	{
		notifyUsers(ERR_USER_NOT_REGISTERED(std::string("*")), fd);
	}
	if (!user->getUser().empty())
	{
		notifyUsers(ERR_ALREADY_REGISTERED(user->getNickname()), fd);
		return;
	}
	if (user)
	{
		user->setUser(param[1]);
		std::string	realname = param[4].substr(1);
		user->setRealname(realname);
	}

	if (user && user->isRegistered() && !user->getUser().empty() &&
		!user->getNickname().empty() && user->getNickname() != "*" &&
		!user->isConnected())
	{
		user->setConnected(true);
		notifyUsers(RPL_SERVER_WELCOME(user->getNickname()), fd);
	}
}