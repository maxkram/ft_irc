#include "../inc/irc.hpp"

Server::Server(int port, std::string password) {
    _port = port;
    _name = "Lit Server";
    _password = password;
	isExit = false;
    init();
}

Server::~Server() {
    std::cout << "Closing server" << std::endl;
    close(_server.fd);
}

Server::Server(Server const & src) {
    *this = src;
}

Server & Server::operator=(Server const & src) {
    if (this != &src) {
        _port = src._port;
        _name = src._name;
        _password = src._password;
    }
    return *this;
}

void Server::init() {
    std::cout << "Initializing..." << std::endl;
    _server.fd = socket(AF_INET, SOCK_STREAM, 0);
    if (_server.fd < 0)
        throw std::runtime_error("Error establishing the socket...");
    int setting = fcntl(_server.fd, F_GETFL, 0);
    fcntl(_server.fd, F_SETFL, setting | O_NONBLOCK);

    _server.addr.sin_family = AF_INET;
    if (_port < 1500)
        throw std::runtime_error("Invalid port number, should be > 1500");
    _server.addr.sin_port = htons(_port);
    _server.addr.sin_addr.s_addr = htonl(INADDR_ANY);

    std::cout << "Binding..." << std::endl;
    if (bind(_server.fd, (struct sockaddr *) &_server.addr, sizeof(_server.addr)) < 0)
        throw std::runtime_error("Error binding");

    std::cout << "Listening..." << std::endl;
    if (listen(_server.fd, SOMAXCONN) < 0)
        throw std::runtime_error("Error listening");

    _pollfds = new std::vector<pollfd>(maxClients + 1);
    std::cout << "Initialized" << std::endl;
}

void Server::socket_polling() {
    std::vector<pollfd> &connectionFds = *_pollfds;

    int num_ready = poll(connectionFds.data(), _num_clients + 1, -1);
    if (num_ready < 0)
        throw std::runtime_error("Error polling");

}

void Server::connect() {
    std::vector<pollfd> &connectionFds = *_pollfds;
    if (!(connectionFds[0].revents & POLLIN))
        return;

    socklen_t size = sizeof(_client.addr);

    _client.fd = accept(_server.fd, (struct sockaddr *) &_client.addr, &size);
    if (_client.fd == -1)
        throw std::runtime_error("Accepting failed");

    int setting = fcntl(_client.fd, F_GETFL, 0);
    fcntl(_client.fd, F_SETFL, setting | O_NONBLOCK);

    if (_num_clients == maxClients)
        throw std::runtime_error("Too many clients");

	connectionFds[_num_clients + 1].fd = _client.fd;
    connectionFds[_num_clients + 1].events = POLLIN | POLLOUT;

    _num_clients++;
}

void Server::read_client()
{
    std::vector<pollfd> &connectionFds = *_pollfds;

    for (int i = 1; i <= _num_clients; i++)
    {
        if (connectionFds[i].fd != -1 && connectionFds[i].revents & POLLIN)
        {
            static std::string completeMessage;
            memset(&completeMessage, '\0', sizeof(completeMessage));
            std::cout << "Reading... here" << std::endl;
            while (!bufferEndMessage(completeMessage))
            {

                char buf[10];
                memset(buf, 0, sizeof(buf));
                int bytes = recv(connectionFds[i].fd, buf, sizeof(buf), 0);
                if (bytes > 0) 
                    completeMessage += std::string(buf, bytes);
                else if (bytes == -1)
                {
                    if (errno == EWOULDBLOCK || errno == EAGAIN) {
                        throw std::runtime_error("Error nothing to read anymore");
                    }
                    else
                        throw std::runtime_error("Error reading inside loop");
                }
                else if (bytes == 0)
                    throw std::runtime_error("Error in the reading, bytes == 0");
                }
                Message::execMessage(completeMessage);
                std::cout << "next client connection \n" << std::endl;
        }
    }
}

int    Server::bufferEndMessage(std::string completeMessage)
{
    size_t found = completeMessage.find("test");
    if (found != std::string::npos)
        std::cout << "found " << std::endl;
    return (found != std::string::npos) ? 1 : 0;
}

std::string Server::readMessage(std::string completeMessage, const pollfd &connectionFds)
{
    std::cout << "Entering readMessage" << std::endl;
    char buf[10];
    memset(buf, 0, sizeof(buf));
    int bytes = recv(connectionFds.fd, buf, sizeof(buf), 0);
    if (bytes > 0) 
    {
        completeMessage += std::string(buf, bytes);
        std::cout << "Received: " << buf << std::endl;
        std::cout << "Message is now : " << completeMessage << std::endl;
        return (completeMessage);
    }
    else if (bytes == -1)
    {
        if (errno == EWOULDBLOCK || errno == EAGAIN) {
            throw std::runtime_error("Error nothing to read anymore");
        }
        else
            throw std::runtime_error("Error reading inside loop");
    }
    else if (bytes == 0)
        throw std::runtime_error("Error in the reading, bytes == 0");
    std::cout << "return";
    return (completeMessage);
}

void Server::launchServer() {
    std::vector<pollfd> &connectionFds = *_pollfds;
    connectionFds[0].fd = _server.fd;
    connectionFds[0].events = POLLIN;

    _num_clients = 0;

    std::cout << "Launching..." << std::endl;
    while (isExit == false) {
		try {
            socket_polling();
            connect();
            // send_message();
            read_client();
        }
        catch (std::exception &e) {
            std::cerr << "Error: " << e.what() << std::endl;
            exit(EXIT_FAILURE);
        }
    }
}

void Server::execute(std::string &message) {
    size_t pos = message.find("\r\n");
    if (pos == std::string::npos)
        return;
    std::string command = message.substr(0, pos);
    message.erase(0, pos + 2);
}
