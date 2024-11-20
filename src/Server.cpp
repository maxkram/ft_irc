#include "../inc/irc.hpp"

#define RESET   "\033[0m"
#define CYAN    "\033[36m"
#define GREEN   "\033[32m"
#define YELLOW  "\033[33m"
#define RED     "\033[31m"

Server::Server(int port, std::string password) {
    _port = port;
    _name = "Lit Server";
    isExit = false;
    _password = password;
    init();
}

Server::~Server() {
    std::cout << YELLOW << "Shutting down the server. Closing file descriptor: " << _server.fd << RESET << std::endl;
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
    std::cout << CYAN << "Initializing server on port " << _port << "..." << RESET << std::endl;

    _server.fd = socket(AF_INET, SOCK_STREAM, 0);
    if (_server.fd < 0)
        throw std::runtime_error(RED "Failed to create a socket. Aborting." RESET);

    int setting = fcntl(_server.fd, F_GETFL, 0);
    fcntl(_server.fd, F_SETFL, setting | O_NONBLOCK);

    _server.addr.sin_family = AF_INET;

    if (_port < 1500)
        throw std::runtime_error(RED "Invalid port number: must be greater than 1500." RESET);
    
    _server.addr.sin_port = htons(_port);
    _server.addr.sin_addr.s_addr = htonl(INADDR_ANY);

    std::cout << CYAN << "Binding server socket to port " << _port << "..." << RESET << std::endl;
    if (bind(_server.fd, (struct sockaddr *) &_server.addr, sizeof(_server.addr)) < 0)
        throw std::runtime_error(RED "Failed to bind the server socket." RESET);

    std::cout << CYAN << "Setting up the server to listen for incoming connections..." << RESET << std::endl;
    if (listen(_server.fd, SOMAXCONN) < 0)
        throw std::runtime_error(RED "Failed to set the server to listening mode." RESET);

    _pollfds = new std::vector<pollfd>(maxClients + 1);
    std::cout << GREEN << "Server initialization complete. Ready to accept connections." << RESET << std::endl;
}

void Server::socket_polling() {
    std::vector<pollfd> &connectionFds = *_pollfds;

    int num_ready = poll(connectionFds.data(), _num_clients + 1, -1);
    if (num_ready < 0)
        throw std::runtime_error(RED "Error during poll. Failed to monitor file descriptors." RESET);
        std::cout << connectionFds[_num_clients + 1].events << std::endl;
        std::cout << connectionFds[_num_clients + 1].revents << std::endl;        
}

void Server::connect() {
    std::vector<pollfd> &connectionFds = *_pollfds;
    if (!(connectionFds[0].revents & POLLIN))
        return;

    socklen_t size = sizeof(_client.addr);

    _client.fd = accept(_server.fd, (struct sockaddr *) &_client.addr, &size);
    if (_client.fd < 0)
        throw std::runtime_error(RED "Error accepting a new client connection." RESET);

    int setting = fcntl(_client.fd, F_GETFL, 0);
    fcntl(_client.fd, F_SETFL, setting | O_NONBLOCK);

    if (_num_clients == maxClients)
        throw std::runtime_error(RED "Maximum client limit reached. Unable to accept more clients." RESET);

    connectionFds[_num_clients + 1].fd = _client.fd;
    connectionFds[_num_clients + 1].events = POLLIN | POLLOUT;

    _num_clients++;
    std::cout << GREEN << "New client connected. Total clients: " << _num_clients << RESET << std::endl;
}

void Server::read_client() {
    char buffer[4096];
    memset(buffer, 0, sizeof(buffer));
    int bytes = 0;
    std::vector<pollfd> &connectionFds = *_pollfds;

    for (int i = 1; i < _num_clients + 1; i++) {
        if (connectionFds[i].fd != -1 && connectionFds[i].revents & POLLIN) {
            std::cout << CYAN << "Reading data from client (FD: " << connectionFds[i].fd << ")..." << RESET << std::endl;
            bytes = recv(connectionFds[i].fd, buffer, sizeof(buffer), 0);

            if (bytes == -1) {
                if (errno == EWOULDBLOCK || errno == EAGAIN) {
                    continue;
                } else {
                    throw std::runtime_error(RED "Error reading from client socket." RESET);
                }
            }
            if (bytes == 0)
                throw std::runtime_error(RED "Client disconnected unexpectedly (FD: " + std::to_string(connectionFds[i].fd) + ")." RESET);

            std::istringstream iss(buffer);
            std::string message;
            while (std::getline(iss, message)) {
                std::cout << GREEN << "Client message: " << message << RESET << std::endl;
            }
        }
    }
}

void Server::launchServer() {
    std::vector<pollfd> &connectionFds = *_pollfds;
    connectionFds[0].fd = _server.fd;
    connectionFds[0].events = POLLIN;

    _num_clients = 0;

    std::cout << CYAN << "Starting server main loop..." << RESET << std::endl;

    while (!isExit) {
        try {
            socket_polling();
            connect();
            std::cout << "next : reading" << std::endl;
            read_client();
        } catch (std::exception &e) {
            std::cerr << RED << "Critical Error: " << e.what() << RESET << std::endl;
            exit(EXIT_FAILURE);
        }
    }
}
