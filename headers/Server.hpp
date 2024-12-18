#pragma once

# include "Client.hpp"
# include "Command.hpp"
# include "messages.hpp"

# include <netinet/in.h>
# include <errno.h>
# include <unistd.h>
# include <iostream>
# include <list>
# include <vector>
# include <string>
# include <stdexcept>
# include <sys/select.h>
# include <fcntl.h>
# include <sys/socket.h>
# include <arpa/inet.h>
# include <netdb.h>

class	Server
{
	public:
			// Constructor & Destructor
			Server(int port, std::string const & pass);
			Server();
			~Server();
			// Core methods
			void	findHostname();
			void	initialize();
			void	launch();

	private:
			const int	port;
			const std::string	pass;
			std::string	serverIP;
			std::list<Client>	clients;
			Command	command; 

			int	serverSocket;
			struct sockaddr_in serverAddress;
			int	addressLength;
			fd_set	readfds;
};
