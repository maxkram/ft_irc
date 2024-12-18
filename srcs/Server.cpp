#include "../headers/Server.hpp"
// Constructor with parameters
Server::Server(int port, std::string const & pass) : port(port), pass(pass), command(pass, clients, serverIP)
{
	findHostname();
	command.setIP(serverIP);
}
// Default constructor
Server::Server() : port(4242), pass(""), command(pass, clients, serverIP)
{
	findHostname();
	command.setIP(serverIP);
}
// Destructor
Server::~Server()
{
	close(serverSocket);
}
// Resolve the server's hostname and IP address
void	Server::findHostname()
{
	char	host[256];
	struct hostent	*hostEntry;
	if (gethostname(host, sizeof(host)) == -1)
        throw std::runtime_error("gethostname failed");
	// gethostname(host, sizeof(host));
	hostEntry = gethostbyname(host);
	if (!hostEntry)
        throw std::runtime_error("gethostbyname failed");
	serverIP = inet_ntoa(*((struct in_addr *)hostEntry->h_addr_list[0]));
}
// Initialize the server socket and configuration
void	Server::initialize()
{
	int	opt = 1;
	// Create a socket
	serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket == 0)
        throw std::runtime_error("Failed to create socket");
	// Set socket options
	if(setsockopt(serverSocket, SOL_SOCKET, SO_REUSEADDR, (char *)&opt, sizeof(opt)) < 0)  
		throw std::runtime_error("setsockopt failed");
	// Configure the address structure
	serverAddress.sin_family = AF_INET;
	serverAddress.sin_addr.s_addr = INADDR_ANY;
	serverAddress.sin_port = htons(port);
	// Set the socket to non-blocking
	if (fcntl(serverSocket, F_SETFL, O_NONBLOCK) < -1)
		throw std::runtime_error("fcntl failed");
	// Bind the socket
	if (bind(serverSocket, (struct sockaddr *)&serverAddress, sizeof(serverAddress))<0)  
		throw std::runtime_error("Bind failed");
	// Listen for connections
	if (listen(serverSocket, 3) < 0)  
		throw std::runtime_error("Listen failed");
	addressLength = sizeof(serverAddress);
}
// Main server loop
void	Server::launch()
{
	int	sd;
	int	max;
	int	res;
	std::list<Client>::iterator	it;
	char	buf[2048];

	while(true)
	{
		FD_ZERO(&readfds);
		FD_SET(serverSocket, &readfds);  
		max = serverSocket;
		// Add client sockets to the readfds set
		for (it = clients.begin(); it != clients.end(); it++)
		{
			if (it->isMarkedForDeletion())
				it = clients.erase(it);
			sd = it->getSocket();		
			FD_SET(sd, &readfds);
			if (sd > max)
				max = sd;
		}
		res = select(max + 1 , &readfds , NULL , NULL , NULL);  
		if ((res < 0) && (errno!=EINTR))  
			throw std::runtime_error("select error");
		if (FD_ISSET(serverSocket, &readfds))  
		{
			if ((res = accept(serverSocket, (struct sockaddr *)&serverAddress, (socklen_t*)&addressLength))<0)  
				throw std::runtime_error("accept");
			Client	client(res, inet_ntoa(serverAddress.sin_addr), ntohs(serverAddress.sin_port));
			std::string	message(":" + serverIP + " NOTICE * :IRC : To register please use commands PASS - NICK - USER(user, mode, unused, realname)\r\n");
			send(client.getSocket(), message.c_str(), message.length(), 0);
			clients.push_back(client);
		}
		for (it = clients.begin(); it != clients.end(); it++)
		{
			sd = it->getSocket();
			if (FD_ISSET(sd, &readfds))
			{
				if ((res = recv(sd, (void*)buf, 2048, 0)) == 0)
				{
					it->setConnected(false);
					it->getBuffer().assign("QUIT :Remote host closed the connection\r\n");
				}
				else
				{
					buf[res] = '\0';
					it->getBuffer().append(buf);
				}
				if (it->getBuffer()[it->getBuffer().length() - 1] == '\n')
				{
					command.parseCommand(*it);
					it->getBuffer().clear();
				}
				bzero(buf, res);
			}
		}
	}
}
