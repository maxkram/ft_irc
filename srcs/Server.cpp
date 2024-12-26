#include "../includes/server.hpp"

Server::Server()
{
	std::cout << GREEN << "-------------- SERVER --------------" << RESET << std::endl;
	this->sock_fd = -1;
	sock_opt = 1;
	poll_nb = 10;
	user_max = 10;
}

Server::Server(Server const &obj)
{
	*this = obj;
}

Server &Server::operator=(Server const &obj){
	if (this != &obj)
	{
		this->sock_fd = obj.sock_fd;
		this->port = obj.port;
		this->password = obj.password;
		this->sock_user = obj.sock_user;
		this->poll_fd = obj.poll_fd;
	}
	return *this;
}

Server::~Server()
{
	std::cout << RED << "----------- SERVER CLOSED -----------" << RESET << std::endl;
}

int	Server::getSockFd()
{
	return (this->sock_fd);
}

int	Server::getPort()
{
	return (this->port);
}

int	Server::getFdByNick(std::string nickname)
{
	int	fd;

	for (size_t i = 0; i < sock_user.size(); i++)
	{
		if (nickname == sock_user[i].getNickname())
		{
			fd = sock_user[i].getFduser();
			return (fd);
		}
	}
	return (-1);
}

std::string	Server::getPassword()
{
	return (this->password);
}

User	*Server::getClientByFd(int fd)
{
	for (size_t i = 0; i < sock_user.size(); i++)
	{
		if (this->sock_user[i].getFduser() == fd)
			return (&this->sock_user[i]);
	}
	return (NULL);
}

User	*Server::getClientByNickname(std::string nickname)
{
	for (size_t i = 0; i < sock_user.size(); i++)
	{
		if (this->sock_user[i].getNickname() == nickname)
			return (&this->sock_user[i]);
	}
	return (NULL);
}

Channel	*Server::getChannel(std::string name)
{
	for (size_t i = 0; i < this->channel.size(); i++)
	{
		if (this->channel[i].getChannelName() == name)
			return (&channel[i]);
	}
	return (NULL);
}

void Server::setSockFd(int fd)
{
    this->sock_fd = fd;
}

void Server::setPort(int port)
{
    this->port = port;
}

void Server::setPassword(std::string password)
{
    this->password = password;
}

void Server::setNewUser(User newuser)
{
    this->sock_user.push_back(newuser);
}

void Server::setNewChannel(Channel newchannel)
{
    this->channel.push_back(newchannel);
}

void Server::setPollfd(pollfd fd)
{
    this->poll_fd.push_back(fd);
}

void Server::removeClient(int fd)
{
    for (size_t i = 0; i < this->sock_user.size(); i++)
    {
        if (this->sock_user[i].getFduser() == fd)
        {
            this->sock_user.erase(this->sock_user.begin() + i);
            return;
        }
    }
}

void Server::removeFd(int fd)
{
    for (size_t i = 0; i < this->poll_fd.size(); i++)
    {
        if (this->poll_fd[i].fd == fd)
        {
            this->poll_fd.erase(this->poll_fd.begin() + i);
            return;
        }
    }
}

void Server::clearChannel(int fd)
{
    int flag;
    std::string reply;

    for (size_t i = 0; i < this->channel.size(); i++)
    {
        flag = 0;
        if (channel[i].getUserByFd(fd))
        {
            channel[i].removeUserByFd(fd);
            flag = 1;
        }
        else if (channel[i].getOperatorByFd(fd))
        {
            channel[i].removeOperatorByFd(fd);
            flag = 1;
        }
        if (channel[i].getUserCount() == 0)
        {
            channel.erase(channel.begin() + i);
            i--;
            continue;
        }
        if (flag)
        {
            reply = ":" + getClientByFd(fd)->getNickname() + "!~" + getClientByFd(fd)->getUser() + "@localhost QUIT Quit\r\n";
            channel[i].broadcastMessage(reply);
        }
    }
}

void Server::notifyUsers(std::string message, int fd)
{
    std::cout << GREEN << RARROW << RESET << message;
    if (send(fd, message.c_str(), message.size(), 0) == -1)
        std::cerr << "send() failed" << std::endl;
}

void Server::notifyClient2(int errnum, std::string user, std::string channel, int fd, std::string message)
{
    std::stringstream ss;
    std::string rep;

    ss << ":localhost " << errnum << " " << user << " " << channel << message;
    rep = ss.str();
    if (send(fd, rep.c_str(), rep.size(), 0) == -1)
        std::cerr << "send() failed" << std::endl;
}

void Server::notifyClient3(int errnum, std::string user, int fd, std::string message)
{
    std::stringstream ss;
    std::string rep;

    ss << ":localhost " << errnum << " " << user << " " << message;
    rep = ss.str();
    if (send(fd, rep.c_str(), rep.size(), 0) == -1)
        std::cerr << "send() failed" << std::endl;
}

void Server::closeFd()
{
    for (size_t i = 0; i < sock_user.size(); i++)
    {
        std::cout << "FD[" << sock_fd << "] disconnected" << std::endl;
        close(sock_user[i].getFduser());
    }
    if (sock_fd != -1)
    {
        std::cout << "Server (FD[" << sock_fd << "]) disconnected" << std::endl;
        close(sock_fd);
    }
}

bool Server::isPortValid(std::string port)
{
    return (port.find_first_not_of("0123456789") == std::string::npos &&
            atoi(port.c_str()) >= 1024 && atoi(port.c_str()) <= 65535);
}

bool Server::isRegistered(int fd)
{
    if (!getClientByFd(fd) || getClientByFd(fd)->getNickname().empty() || 
        getClientByFd(fd)->getUser().empty() || getClientByFd(fd)->getNickname() == "*" || 
        !getClientByFd(fd)->isConnected())
        return (false);
    return (true);
}

bool Server::isChannelAvailable(std::string channelName)
{
    if (channelName.empty() || channelName.size() <= 2 || channelName[0] != '#')
        return (false);
    
    std::string chan = channelName.substr(1);
    for (std::vector<Channel>::iterator it = channel.begin(); it != channel.end(); ++it)
    {
        if (it->getChannelName() == chan)
            return (true);
    }
    return (false);
}
