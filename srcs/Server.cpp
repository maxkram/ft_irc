#include "../includes/server.hpp"

Server::Server() {
	std::cout << GREEN << "=== Initializing Server: Ready to accept connections ===" << RESET << std::endl;
	this->serverSocketFd = -1;
	socketOptions = 1;
	maxPollEvents = 10;
	maxUsers = 10;
}

Server::Server(Server const &obj) {
    *this = obj;
    std::cout << GREEN << "=== Server Instance Copied Successfully ===" << RESET << std::endl;
}

Server &Server::operator=(Server const &obj) {
	if (this != &obj) {
		this->serverSocketFd = obj.serverSocketFd;
		this->port = obj.port;
		this->password = obj.password;
		this->connectedUsers = obj.connectedUsers;
		this->pollFds = obj.pollFds;
        std::cout << GREEN << "=== Server State Assigned Successfully ===" << RESET << std::endl;
	}
	return *this;
}

Server::~Server() {
	std::cout << RED << "=== Shutting Down Server: All connections closed ===" << RESET << std::endl;
}

int	Server::getServerSocketFd() {
	return (this->serverSocketFd);
}

int	Server::getServerPort() {
	return (this->port);
}

int	Server::getSocketFdByNickname(std::string nickname) {
	for (size_t i = 0; i < connectedUsers.size(); ++i) {
		if (nickname == connectedUsers[i].getNickname())
			return connectedUsers[i].getFduser();
	}
	return (-1);
}

std::string	Server::getServerPassword() { return (this->password); }

User	*Server::getClientByFd(int fd) {
	for (size_t i = 0; i < connectedUsers.size(); ++i) {
		if (connectedUsers[i].getFduser() == fd)
			return (&connectedUsers[i]);
	}
	return (NULL);
}

User	*Server::getClientByNickname(std::string nickname) {
	for (size_t i = 0; i < connectedUsers.size(); ++i) {
		if (connectedUsers[i].getNickname() == nickname)
			return (&connectedUsers[i]);
	}
	return (NULL);
}

Channel	*Server::getChannel(std::string name) {
	for (size_t i = 0; i < channel.size(); ++i) {
		if (channel[i].getChannelName() == name)
			return (&channel[i]);
	}
	return (NULL);
}

void Server::setServerSocketFd(int fd) { this->serverSocketFd = fd; }

void Server::setServerPort(int port) { this->port = port; }

void Server::setServerPassword(std::string password) { this->password = password; }

void Server::setNewUser(User newuser) { this->connectedUsers.push_back(newuser); }

void Server::setNewChannel(Channel newchannel) { this->channel.push_back(newchannel); }

void Server::setPollfd(pollfd fd) { this->pollFds.push_back(fd); }

void Server::removeClient(int fd) {
    for (size_t i = 0; i < connectedUsers.size(); ++i) {
        if (connectedUsers[i].getFduser() == fd) {
            connectedUsers.erase(connectedUsers.begin() + i);
            return;
        }
    }
}

void Server::removePollFd(int fd)
{
    for (size_t i = 0; i < pollFds.size(); ++i) {
        if (pollFds[i].fd == fd) {
            pollFds.erase(pollFds.begin() + i);
            return;
        }
    }
}

void Server::clearChannel(int fd) {
    std::string reply;

    for (size_t i = 0; i < channel.size(); ++i) {
        bool userRemoved = false;

        // Check if the file descriptor corresponds to a regular user
        if (channel[i].getUserByFd(fd)) {
            channel[i].removeUserByFd(fd);
            userRemoved = true;
        }
        // Check if the file descriptor corresponds to an operator
        else if (channel[i].getOperatorByFd(fd)) {
            channel[i].removeOperatorByFd(fd);
            userRemoved = true;
        }

        // Remove empty channels
        if (channel[i].getUserCount() == 0) {
            channel.erase(channel.begin() + i);
            --i; // Adjust index after removal
            continue;
        }

        // Notify users in the channel if a user was removed
        if (userRemoved) {
            User *client = getClientByFd(fd);
            if (client) {
                reply = ":" + client->getNickname() + "!~" + client->getUser() + "@localhost QUIT Quit\r\n";
                channel[i].broadcastMessage(reply);
            }
            else {
                std::cerr << "Error: Unable to retrieve client information for fd " << fd << std::endl;
            }
        }
    }
}

void Server::notifyUsers(std::string message, int fd)
{
    std::cout << GREEN << RARROW << RESET << message;
    if (send(fd, message.c_str(), message.size(), 0) == -1)
        std::cerr << "Error: Failed to send message to fd " << fd << std::endl;
}

void Server::sendErrorToClient(int errnum, std::string user, std::string channel, int fd, std::string message)
{
    // Construct the error response
    std::ostringstream responseStream;
    responseStream << ":localhost " << errnum << " " << user << " " << channel << " " << message;
    std::string response = responseStream.str();

    // Log the error being sent
    std::cout << RED << "Sending error to fd " << fd << ": " << RESET << response;

    // Send the error message
    if (send(fd, response.c_str(), response.size(), 0) == -1) {
        std::cerr << "Error: Failed to send error message to fd " << fd << std::endl;
    }
    else {
        std::cout << GREEN << "Error message sent successfully to fd " << fd << RESET << std::endl;
    }
}

void Server::sendError(int errnum, std::string user, int fd, std::string message)
{
    // Construct the error message
    std::ostringstream responseStream;
    responseStream << ":localhost " << errnum << " " << user << " " << message;
    std::string response = responseStream.str();

    // Log the error message being sent
    std::cout << RED << "Sending error to fd " << fd << ": " << RESET << response;

    // Send the error message to the client
    if (send(fd, response.c_str(), response.size(), 0) == -1) {
        std::cerr << "Error: Failed to send error message to fd " << fd << std::endl;
    }
    else {
        std::cout << GREEN << "Error message sent successfully to fd " << fd << RESET << std::endl;
    }
}

void Server::closeFd()
{
    // Close all connected user sockets
    for (size_t i = 0; i < connectedUsers.size(); ++i) {
        int userFd = connectedUsers[i].getFduser();
        if (close(userFd) == -1) {
            std::cerr << "Error: Failed to close user FD[" << userFd << "]." << std::endl;
        }
        else {
            std::cout << "User FD[" << userFd << "] disconnected successfully." << std::endl;
        }
    }

    // Close the server socket
    if (serverSocketFd != -1) {
        if (close(serverSocketFd) == -1) {
            std::cerr << "Error: Failed to close server FD[" << serverSocketFd << "]." << std::endl;
        }
        else {
            std::cout << "Server (FD[" << serverSocketFd << "]) disconnected successfully." << std::endl;
        }
    }
}


bool Server::validatePort(std::string port) 
{
    return (port.find_first_not_of("0123456789") == std::string::npos &&
            atoi(port.c_str()) >= 1024 && atoi(port.c_str()) <= 65535);
}

bool Server::isRegistered(int fd)
{
    User *client = getClientByFd(fd);

    // Check if the client exists
    if (!client)
    {
        return false;
    }

    // Validate client registration criteria
    if (client->getNickname().empty() || client->getUser().empty() || 
        client->getNickname() == "*" || !client->isConnected())
    {
        return false;
    }

    return true;
}

bool Server::isChannelAvailable(std::string channelName)
{
    // Validate channel name format
    if (channelName.empty() || channelName.size() <= 2 || channelName[0] != '#')
    {
        return false;
    }

    // Extract the actual channel name (excluding the '#' prefix)
    std::string chan = channelName.substr(1);

    // Check if the channel exists
    for (std::vector<Channel>::iterator it = channel.begin(); it != channel.end(); ++it)
    {
        if (it->getChannelName() == chan)
        {
            return true;
        }
    }

    return false;
}
