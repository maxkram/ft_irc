#include "../includes/server.hpp"
#include <cstdlib>  // For std::strtol
#include <cstring>  // For std::strlen
#include <iostream> // For std::cerr

// Helper function to validate the port
bool isPortValid(const char *portStr)
{
    char *endptr = NULL;
    long port = std::strtol(portStr, &endptr, 10);

    // Check if the entire string was converted and the port is in a valid range
    return *endptr == '\0' && port > 0 && port <= 65535;
}

// Helper function to validate the password
bool isPasswordValid(const char *password)
{
    return password != NULL && std::strlen(password) > 0 && std::strlen(password) <= 20;
}

int main(int argc, char **argv)
{
    Server serv;

    // Validate argument count
    if (argc != 3)
    {
        std::cerr << "Error: Syntax must be ./ircserv <port> <password>\n";
        return 1;
    }

    const char *portStr = argv[1];
    const char *password = argv[2];

    try
    {
        // Register signal handlers
        int signals[] = {SIGINT, SIGQUIT};
        for (size_t i = 0; i < sizeof(signals) / sizeof(signals[0]); ++i)
        {
            signal(signals[i], Server::handleSignal);
        }

        // Validate port and password
        if (!isPortValid(portStr))
        {
            std::cerr << "Error: Invalid port number! Must be an integer between 1 and 65535.\n";
            return 1;
        }

        if (!isPasswordValid(password))
        {
            std::cerr << "Error: Invalid password! Must be between 1 and 20 characters.\n";
            return 1;
        }

        // Initialize the server
        serv.initServer(std::atoi(portStr), password);

        // Begin processing events
        serv.checkPollEvents();
    }
    catch (const std::exception &e)
    {
        serv.closeFd(); // Ensure resources are released
        std::cerr << "Error: " << e.what() << std::endl;
        return 1; // Return error code
    }

    return 0; // Return success code
}
