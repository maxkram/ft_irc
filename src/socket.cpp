//#include "../inc/socket.hpp"
#include "../inc/irc.hpp"

Socket::Socket(int domain, int service, int protocol, int port, u_long interface)
{
    address.sin_family = domain;
    address.sin_port = htons(port);
    address.sin_addr.s_addr = htonl(interface);
    sock = socket(domain, service, protocol);
    testConnection(sock);
}

void Socket::testConnection(int toTest)
{
    if (toTest < 0)
    {
        std::cerr << "Failed to connect..." << errno << std::endl;
        exit(EXIT_FAILURE);
    }
}

struct sockaddr_in Socket::getAddress()
{
    return address;
}

int Socket::getSock()
{
    return sock;
}