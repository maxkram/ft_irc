#include "../inc/irc.hpp"
#include <signal.h>

bool g_isExit = false;

void signal_handler(int sig) {
    if (sig == SIGINT)
    {
        std::cout << "end of program" << std::endl;
        g_isExit = true;
    }
}

int main(int argc, char **argv)
{
    if (argc != 3) {
        std::cerr << "Usage: ./ircserv  <port> <password>" << std::endl;
        exit(EXIT_FAILURE);
    }
w    signal(SIGINT, signal_handler);
    try
    {
        int port = std::atoi(argv[1]);
        std::string password = argv[2];
        Server server(port, password);
        while (!g_isExit)
            server.launchServer();
        std::cout << "reached" << std::endl;
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
    }
    return 0;
}