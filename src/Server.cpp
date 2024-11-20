#include "../inc/irc.hpp"

#define RED     "\033[31m"
#define GREEN   "\033[32m"
#define YELLOW  "\033[33m"
#define BLUE    "\033[34m"
#define MAGENTA "\033[35m"
#define CYAN    "\033[36m"
#define RESET   "\033[0m"

Server::Server(int port, std::string password) {
    _port = port;
    _name = "Lit Server";
    _password = password;
    init();
}

Server::~Server() {
    std::cout << RED << "[SERVER] Shutting down..." << RESET << std::endl;
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
    std::cout << BLUE << "[SERVER] Initializing server on port " << _port << "..." << RESET << std::endl;
    
    _server.fd = socket(AF_INET, SOCK_STREAM, 0);
    if (_server.fd < 0)
        throw std::runtime_error(RED "Failed to create socket descriptor" RESET);
    
    int setting = fcntl(_server.fd, F_GETFL, 0);
    fcntl(_server.fd, F_SETFL, setting | O_NONBLOCK);
    std::cout << GREEN << "[SERVER] Socket created successfully" << RESET << std::endl;

    _server.addr.sin_family = AF_INET;
    _server.addr.sin_port = htons(_port);
    _server.addr.sin_addr.s_addr = htonl(INADDR_ANY);

    std::cout << BLUE << "[SERVER] Binding to port " << _port << "..." << RESET << std::endl;
    if (bind(_server.fd, (struct sockaddr *) &_server.addr, sizeof(_server.addr)) < 0)
        throw std::runtime_error(RED "Failed to bind socket to port" RESET);
    std::cout << GREEN << "[SERVER] Binding successful" << RESET << std::endl;

    std::cout << BLUE << "[SERVER] Starting to listen for connections..." << RESET << std::endl;
    if (listen(_server.fd, 1) < 0)
        throw std::runtime_error(RED "Failed to listen on socket" RESET);
    std::cout << GREEN << "[SERVER] Listening successfully" << RESET << std::endl;

    _pollfds = new std::vector<pollfd>(maxClients + 1);
    std::cout << GREEN << "[SERVER] Server initialized successfully" << RESET << std::endl;
}

void Server::socket_polling() {
    std::vector<pollfd> &connectionFds = *_pollfds;

    int num_ready = poll(connectionFds.data(), _num_clients + 1, -1);
    if (num_ready < 0)
        throw std::runtime_error(RED "Polling operation failed" RESET);
}

void Server::connect() {
    std::vector<pollfd> &connectionFds = *_pollfds;
    if (!(connectionFds[0].revents & POLLIN))
        return;

    socklen_t size = sizeof(_client.addr);

    _client.fd = accept(_server.fd, (struct sockaddr *) &_client.addr, &size);
    if (_client.fd < 0)
        throw std::runtime_error(RED "Failed to accept new connection" RESET);

    int setting = fcntl(_client.fd, F_GETFL, 0);
    fcntl(_client.fd, F_SETFL, setting | O_NONBLOCK);

    if (_num_clients == maxClients)
        throw std::runtime_error(RED "Server has reached maximum client capacity" RESET);

    char buffer[1024];
    ssize_t receive = recv(_client.fd, buffer, 1024, 0);
    if (receive < 0) {
        throw std::runtime_error(RED "Failed to receive client data" RESET);
    }

    std::cout << CYAN << "[CLIENT] New connection attempt from " 
              << inet_ntoa(_client.addr.sin_addr) << ":" 
              << ntohs(_client.addr.sin_port) << RESET << std::endl;

    std::istringstream iss(buffer);
    std::string line;
    while (std::getline(iss, line)) {
        if (line.compare(0, 5, "PASS ") == 0) {
            std::string password = line.substr(5, line.size() - 6);
            if (password.compare(_password) != 0) {
                std::cout << RED << "[AUTH] Failed authentication attempt from "
                         << inet_ntoa(_client.addr.sin_addr) << RESET << std::endl;
                throw std::runtime_error(RED "Invalid password provided" RESET);
                isExit = true;
                break;
            }
            std::cout << GREEN << "[AUTH] Client authenticated successfully" << RESET << std::endl;
        }
    }

    connectionFds[_num_clients + 1].fd = _client.fd;
    connectionFds[_num_clients + 1].events = POLLIN | POLLOUT;

    _num_clients++;
    std::cout << GREEN << "[SERVER] New client connected. Total clients: " 
              << _num_clients << RESET << std::endl;
}

void Server::read_client() {
    std::vector<pollfd> &connectionFds = *_pollfds;
    for (int i = 1; i < _num_clients + 1; i++) {
        if (connectionFds[i].revents & POLLIN) {
            std::cout << MAGENTA << "[CLIENT " << i << "] Reading incoming message..." << RESET << std::endl;
        }
    }
}

void Server::launchServer() {
    std::vector<pollfd> &connectionFds = *_pollfds;
    connectionFds[0].fd = _server.fd;
    connectionFds[0].events = POLLIN;

    _num_clients = 0;

    std::cout << YELLOW << "\n=== " << _name << " Starting ===" << RESET << std::endl;
    std::cout << BLUE << "[SERVER] Listening on port " << _port << RESET << std::endl;
    
    while (isExit == false) {
        try {
            socket_polling();
            connect();
            if (!isExit)
                read_client();
        }
        catch (std::exception &e) {
            std::cerr << RED << "[ERROR] " << e.what() << RESET << std::endl;
            exit(EXIT_FAILURE);
        }
    }
}