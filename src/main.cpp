#include "../inc/irc.hpp"

int main(int argc, char **argv) 
{
    std::cout << "Starting..." << std::endl;
    std::cout << "Binding socket..." << std::endl;
    Binding bindsocket(AF_INET, SOCK_STREAM, 0, 8888, INADDR_ANY);
    std::cout << "Listening Socket..." << std::endl;
    Listening listensocket(AF_INET, SOCK_STREAM, 0, 8888, INADDR_ANY, 10);
    std::cout << "SUCESS !" << std::endl;

    if (argc != 3) {
        std::cerr << "Usage: ./ircserv  <port> <password>" << std::endl;
        exit(EXIT_FAILURE);
    }
    try {
        int port = std::stoi(argv[1]);
        std::string password = argv[2];
    }
    catch (std::exception &e) {
        std::cerr << "Error: " << e.what() << std::endl;
        exit(EXIT_FAILURE);
    }
}