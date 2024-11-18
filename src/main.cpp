#include "../inc/irc.hpp"

int main(int argc, char **argv) 
{
    std::cout << "Starting..." << std::endl;
    std::cout << "Binding socket..." << std::endl;
    Binding bindsocket(AF_INET, SOCK_STREAM, 0, 8888, INADDR_ANY);
    std::cout << "Listening Socket..." << std::endl;
    Listening listensocket(AF_INET, SOCK_STREAM, 0, 8888, INADDR_ANY, 10);
    std::cout << "SUCESS !" << std::endl; 
}