// #include "../includes/server.hpp"

// int	main(int ac, char **av)
// {
// 	Server	serv;

// 	if (ac != 3)
// 	{
// 		std::cerr << "Error: Syntax must be ./ircserv <port> <password>\n";
// 		return (1);
// 	}
// 	try
// 	{
// 		signal(SIGINT, Server::handleSignal);
// 		signal(SIGQUIT, Server::handleSignal);
// 		if (!serv.isPortValid(av[1]) || !*av[2] || strlen(av[2]) > 21)
// 		{
// 			std::cerr << "Error: Invalid port number / password!" << std::endl;
// 			return (1);
// 		}
// 		serv.initialize(atoi(av[1]), av[2]);
// 		serv.checkPollEvents();
// 	}
// 	catch(const std::exception& e)
// 	{
// 		serv.closeFd();
// 		std::cerr << e.what() << std::endl;
// 		return (0);
// 	}
// 	return (0);
// }

#include "../includes/server.hpp"
#include <iostream>
#include <cstring>
#include <csignal>
#include <cstdlib>
#include <sstream>

// Constants
#define MAX_PASSWORD_LENGTH 20

// Helper function for signal handling setup
void setupSignalHandlers() {
    signal(SIGINT, Server::handleSignal);
    signal(SIGQUIT, Server::handleSignal);
    signal(SIGTERM, Server::handleSignal);
}

// Helper function to parse an integer from a string
bool parsePort(const char* portStr, int& port) {
    std::stringstream ss(portStr);
    ss >> port;

    // Check if the entire string was consumed and port is valid
    if (ss.fail() || !ss.eof() || port < 1 || port > 65535) {
        return false;
    }
    return true;
}

// Helper function for validating inputs
bool validateInputs(const char* portStr, const char* password) {
    // Validate port
    int port;
    if (!parsePort(portStr, port)) {
        std::cerr << "Error: Port number must be a valid integer between 1 and 65535." << std::endl;
        return false;
    }

    // Validate password
    if (!password || std::strlen(password) == 0) {
        std::cerr << "Error: Password cannot be empty." << std::endl;
        return false;
    }
    if (std::strlen(password) > MAX_PASSWORD_LENGTH) {
        std::cerr << "Error: Password cannot exceed " << MAX_PASSWORD_LENGTH << " characters." << std::endl;
        return false;
    }

    return true;
}

int	main(int argc, char **argv)
{
	Server	serv;

    if (argc != 3) {
        std::cerr << "Usage: ./ircserv <port> <password>" << std::endl;
        return 1;
    }

    const char* portStr = argv[1];
    const char* password = argv[2];

    if (!validateInputs(portStr, password)) {
        return 1;
    }

	try
	{
		// Setup signal handlers
        setupSignalHandlers();

        // Parse the port and initialize the server
        int port;
        std::stringstream(portStr) >> port;
        serv.initialize(port, password);

        // Start handling events
        serv.checkPollEvents();

		// if (!serv.isPortValid(argv[1]) || !*argv[2] || strlen(argv[2]) > 21)
		// {
		// 	std::cerr << "Error: Invalid port number / password!" << std::endl;
		// 	return (1);
		// }
		// serv.initialize(atoi(argv[1]), argv[2]);
		// serv.checkPollEvents();
	// } catch(const std::exception& e)
	// {
	// 	serv.closeFd();
	// 	std::cerr << e.what() << std::endl;
	// 	return (0);
	// }

    } catch (const std::exception& e) {
        serv.closeFd(); // Ensure resources are cleaned up
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    } catch (...) {
        serv.closeFd(); // Catch unexpected exceptions
        std::cerr << "Error: Unknown exception occurred." << std::endl;
        return 1;
    }

	return (0);
}