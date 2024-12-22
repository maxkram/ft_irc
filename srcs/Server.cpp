#include "../includes/server.hpp"

// Helper Logging Function
void Server::logMessage(const std::string& message, const std::string& color) const {
    std::cout << color << message << RESET << std::endl;
}

// // Constructor
// Server::Server()
// {
// 	std::cout << GREEN << "-------------- SERVER --------------" << RESET << std::endl;
// 	this->sockFd = -1;
// 	sockOpt = 1;
// 	pollNb = 10;
// 	userMax = 10;
// }

// Constructor
Server::Server() : sockFd(-1), pollNb(10), userMax(10), sockOpt(1) {
    logMessage("Server initialized", GREEN);
}

// Constructeur de copie
Server::Server(Server const &obj) {
	*this = obj;
}

// Opérateur d'affectation
Server &Server::operator=(Server const &obj){
	if (this != &obj)
	{
		this->sockFd = obj.sockFd;
		this->port = obj.port;
		this->password = obj.password;
		this->sock_user = obj.sock_user;
		this->pollFds = obj.pollFds;
	}
	return *this;
}

// // Destructeur
// Server::~Server()
// {
// 	std::cout << RED << "----------- SERVER CLOSED -----------" << RESET << std::endl;
// }

// Destructor
Server::~Server() {
    logMessage("Server shutting down", RED);
    for (size_t i = 0; i < sock_user.size(); ++i) {
        close(sock_user[i].getFduser());
    }
    if (sockFd != -1) {
        close(sockFd);
    }
}

// Getters

// Get the server socket descriptor
int	Server::getSockFd() const { return (this->sockFd); }

// Get the port number the server is listening on
int	Server::getPort() const { return (this->port); }

// Get the server password
const std::string	Server::getPassword() const { return (this->password); }

// Get the file descriptor of a user by their nickname
int	Server::getFdByNick(std::string nickname) {
	int	fd;

	for (size_t i = 0; i < sock_user.size(); i++) {
		if (nickname == sock_user[i].getNickname()) {
			fd = sock_user[i].getFduser();
			return (fd);
		}
	}
	return (-1);
}


// Get a pointer to a user by its file descriptor
User	*Server::getClientByFd(int fd) {
	for (size_t i = 0; i < sock_user.size(); i++) {
		if (sock_user[i].getFduser() == fd)
			return (&sock_user[i]);
	}
	return (NULL);
}

// Get a pointer to a user by their nickname
User	*Server::getUserByNickname(const std::string nickname) {
	for (size_t i = 0; i < sock_user.size(); i++) {
		if (sock_user[i].getNickname() == nickname)
			return (&sock_user[i]);
	}
	return (NULL);
}

// Get a pointer to a channel by name
Channel	*Server::getChannel(const std::string name) {
	for (size_t i = 0; i < channel.size(); i++) {
		if (channel[i].getChannelName() == name)
			return (&channel[i]);
	}
	return (NULL);
}

// Setters
// Set the server socket file descriptor
void Server::setSocketFd(int fd) { sockFd = fd; }

// Set the port on which the server should listen for incoming connections
void Server::setPort(int port) { this->port = port; }

// Set the password needed to connect to the server
void Server::setPassword(const std::string password) { this->password = password; }

// Add a new user to the list of clients connected to the server
void Server::setNewUser(User newuser) { this->sock_user.push_back(newuser); }

// Adds a new channel to the list of channels available on the server
void Server::setNewChannel(Channel newchannel) { this->channel.push_back(newchannel); }

// Add a pollfd structure to the list of descriptors to monitor
void Server::setPollfd(pollfd fd) { this->pollFds.push_back(fd); }

// User and Channel Management

// Remove a user from the client list using their file descriptor
void Server::removeUserByFd(int fd)
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

// Remove a file descriptor from the pollFds watchlist
void Server::removePollFd(int fd)
{
    for (size_t i = 0; i < this->pollFds.size(); i++)
    {
        if (this->pollFds[i].fd == fd)
        {
            this->pollFds.erase(this->pollFds.begin() + i);
            return;
        }
    }
}

// Clean up channels by deleting a specific user and removing empty channels
// void Server::clearEmptyChannels(int fd)
// {
//     int flag;
//     std::string reply;

//     for (size_t i = 0; i < this->channel.size(); i++) {
//         flag = 0;
//         if (channel[i].getUserByFd(fd)) {
//             channel[i].removeUserByFd(fd);
//             flag = 1;
//         }
//         else if (channel[i].getOperatorByFd(fd)) {
//             channel[i].removeOperatorByFd(fd);
//             flag = 1;
//         }
//         if (channel[i].getUserCount() == 0) {
//             channel.erase(channel.begin() + i);
//             i--;
//             continue;
//         }
//         if (flag) {
//             reply = ":" + getClientByFd(fd)->getNickname() + "!~" + getClientByFd(fd)->getUser() + "@localhost QUIT Quit\r\n";
//             channel[i].broadcastMessage(reply);
//         }
//     }
// }

void Server::clearEmptyChannels(int fd) {
    for (size_t i = 0; i < channel.size(); ++i) {
        channel[i].removeUserByFd(fd);
        channel[i].removeOperatorByFd(fd);
        if (channel[i].getUserCount() == 0) {
            channel.erase(channel.begin() + i);
            --i;
        }
    }
}

// // // Send a message to a specific socket (fd);
void Server::notifyUsers(std::string message, int fd) {
    std::cout << GREEN << RARROW << RESET << message;
    if (send(fd, message.c_str(), message.size(), 0) == -1)
        std::cerr << "send() failed" << std::endl;
}

// Format and send a message with an error code and additional information
void Server::notifyClient2(int errnum, std::string user, std::string channel, int fd, std::string message) {
    std::stringstream ss;
    std::string rep;

    ss << ":localhost " << errnum << " " << user << " " << channel << message;
    rep = ss.str();
    if (send(fd, rep.c_str(), rep.size(), 0) == -1)
        std::cerr << "send() failed" << std::endl;
}

// Format and send a message with an error code and additional information, without channel name
void Server::notifyClient3(int errnum, std::string user, int fd, std::string message) {
    std::stringstream ss;
    std::string rep;

    ss << ":localhost " << errnum << " " << user << " " << message;
    rep = ss.str();
    if (send(fd, rep.c_str(), rep.size(), 0) == -1)
        std::cerr << "send() failed" << std::endl;
}

// Close all open file descriptors (sockets) for clients and for the server itself
void Server::closeFd() {
    for (size_t i = 0; i < sock_user.size(); i++)
    {
        std::cout << "FD[" << sockFd << "] disconnected" << std::endl;
        close(sock_user[i].getFduser());
    }
    if (sockFd != -1)
    {
        std::cout << "Server (FD[" << sockFd << "]) disconnected" << std::endl;
        close(sockFd);
    }
}

// Check if a user is registered on the server
bool Server::isRegistered(int fd) {
    if (!getClientByFd(fd) || getClientByFd(fd)->getNickname().empty() || 
        getClientByFd(fd)->getUser().empty() || getClientByFd(fd)->getNickname() == "*" || 
        !getClientByFd(fd)->isConnected())
        return (false);
    return (true);
}

// Check if a channel already exists on the server
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
