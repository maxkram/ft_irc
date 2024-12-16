#include "../headers/Client.hpp"

Client::Client(int sd, char * IP, uint32_t port) : socket(sd), IP(IP), port(port),
				buffer(), nick("*"), pass(""), user("*"), mode("*"), unused("*"), realname("*"),
				connected(true), nicked(false), usered(false), registered(false), beDeleted(false),
				Operator(false) {}

Client::~Client() {}

int	Client::getSocket() const { return(socket); }

void	Client::setSocket(int sd) { socket = sd; }

std::string & Client::getBuffer() { return(buffer); }

std::string const & Client::getIP() const { return(IP); }

uint32_t	Client::getPort() const { return(port); }

std::string const & Client::getNick() const { return(nick); }

void	Client::setNick(std::string const & val) { nick = val; }

std::string const & Client::getPass() const { return(pass); }

void	Client::setPass(std::string const & val) { pass = val; }

std::string const & Client::getUsername() const  { return(user); }

void	Client::setUsername(std::string const & val) { user = val; }

std::string const & Client::getMode() const  { return(mode); }

void	Client::setMode(std::string const & val) { mode = val; }

// std::string const & Client::getUnused() const  { return(unused); }

void	Client::setUnused(std::string const & val) { unused = val; }

std::string const & Client::getRealName() const  { return(realname); }

void	Client::setRealName(std::string const & val) { realname = val; }

bool	Client::isConnected() const { return(connected); }

void	Client::setConnected(bool val) { connected = val; }

bool	Client::isNicked() const { return(nicked); }

void	Client::setNicked(bool val) { nicked = val; }

bool	Client::isUsered() const { return(usered); }

void	Client::setUsered(bool val) { usered = val; }

bool	Client::isRegistered() const { return(registered); }

void	Client::setRegistered(bool val) { registered = val; }

bool	Client::isMarkedForDeletion() const { return(beDeleted); }

void	Client::setMarkedForDeletion(bool val) { beDeleted = val; }

bool	Client::isOperator() const { return(Operator); }

void	Client::setOperator(bool val) { Operator = val; }

std::vector<std::string>	& Client::getChannels() { return(channels); }

bool	Client::isInChannel(std::string const & str) const
{
	std::vector<std::string>::const_iterator	it = channels.begin();
	for (; it != channels.end(); it++)
	{
		if (str == *it)
			return (true);	
	}
	return (false);
}

std::pair<bool, std::string> Client::getAwayStatus() const { return (away); }

void	Client::setAwayStatus(std::pair<bool, std::string> val) 
{
	away.first = val.first;
	away.second = val.second;
}

void	Client::removeChannel(std::string const & str)
{
	std::vector<std::string>::const_iterator	it = channels.begin();
	for (; it != channels.end(); it++)
	{
		if (str == *it)
		{
			channels.erase(it);
			return;
		}
	}
}
