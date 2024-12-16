#include "../headers/Channel.hpp"

Channel::Channel()
    : invite(false),
      topicEnabled(false),
      isPassword(false),
      topic(""),
      password("")
{}

Channel::~Channel() {}

void Channel::addClient(Client *client)
{
	clients.push_back(client);
}

void Channel::addChannelOperator(Client *client)
{
	chanOp.push_back(client);
}

// void Channel::deleteClient(Client *client)
// {
// 	std::vector<Client *>::iterator it;
// 	for (it = clients.begin(); it != clients.end(); it++)
// 	{
// 		if (client == *it)
// 		{
// 			clients.erase(it);
// 			return;
// 		}
// 	}
// 	for (it = chanOp.begin(); it != chanOp.end(); it++)
// 	{
// 		if (client == *it)
// 		{
// 			chanOp.erase(it);
// 			return;
// 		}
// 	}
// }

void Channel::deleteClient(Client *client)
{
    // Remove from clients
    std::vector<Client *>::iterator it = std::find(clients.begin(), clients.end(), client);
    if (it != clients.end()) {
        clients.erase(it);
    } else {
        std::cerr << "Warning: Client not found in the clients list!" << std::endl;
    }

    // Remove from chanOp
    it = std::find(chanOp.begin(), chanOp.end(), client);
    if (it != chanOp.end()) {
        chanOp.erase(it);
    } else {
        std::cerr << "Warning: Client not found in the channel operators list!" << std::endl;
    }
}

void Channel::deleteChannelOperator(Client *client)
{
	std::vector<Client *>::iterator it;
	for (it = chanOp.begin(); it != chanOp.end(); it++)
	{
		if (client == *it)
		{
			chanOp.erase(it);
			return;
		}
	}
}

void	Channel::addInvitedClient(Client *client)
{
	invited.push_back(client);
}

void	Channel::deleteInvitedClient(Client * client)
{
	std::vector<Client *>::iterator it;
	for (it = invited.begin(); it != invited.end(); it++)
	{
		if (client == *it)
		{
			invited.erase(it);
			return;
		}
	}
}

bool	Channel::isClientInvited(Client const & client) const
{
	std::vector<Client *>::const_iterator it = invited.begin();
	for (; it != invited.end(); it++)
		if (*it == &client)
			return true;
	return false;
}

void	Channel::sendChannelMessage(Client const &client, std::string const &cmd, std::string const &opt)
{
	std::string message(":" + CLIENT);
	if (opt.empty())
		message += " " + cmd + "\r\n";
	else
		message += " " + cmd + " :" + opt + "\r\n";
	std::vector<Client *>::iterator it = clients.begin();
	for (; it != clients.end(); it++)
	{
		if (*it != &client)
		{
			send((*it)->getSocket(), message.c_str(), message.length(), 0);
		}
	}
	for (it = chanOp.begin(); it != chanOp.end(); it++)
	{
		if (*it != &client)
		{
			send((*it)->getSocket(), message.c_str(), message.length(), 0);
		}
	}
}

bool Channel::isChannelOperator(Client const &client)
{
	std::vector<Client *>::iterator it = chanOp.begin();
	for (; it != chanOp.end(); it++)
		if (*it == &client)
			return true;
	return false;
}

bool Channel::isChannelEmpty() const
{
	return (clients.empty() && chanOp.empty());
}

bool Channel::isInviteEnabled() { return (invite); }

void Channel::setInviteEnabled(bool set) { invite = set; }

std::string const & Channel::getTopic() { return (topic); }

void Channel::setTopic(std::string const & val, Client & client) 
{
	std::time_t result = std::time(nullptr);
	topic = val;
	topicAuthor.assign(CLIENT + " " + std::to_string((int)result));
}

void Channel::setPassword(std::string const &str) { password = str, isPassword = true; }

bool Channel::hasPassword() const {return (isPassword); }

std::string const & Channel::getPassword(void) const { return (password); }

void  Channel::enableTopic(bool val) { topicEnabled = val; }

bool Channel::isTopicEnabled() { return (topicEnabled); }

std::string Channel::getTopicAuthor() {return (topicAuthor);}
