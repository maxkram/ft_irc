#include "../inc/irc.hpp"

#define COLOR_RESET   "\033[0m"
#define COLOR_RED     "\033[31m"
#define COLOR_GREEN   "\033[32m"
#define COLOR_YELLOW  "\033[33m"
#define COLOR_BLUE    "\033[34m"
#define COLOR_CYAN    "\033[36m"
#define COLOR_MAGENTA "\033[35m"

Server::Server(int port, std::string password) {
    _port = port;
    _name = "Lit Server";
    _password = password;
    isExit = false;
    init();
}

Server::~Server() {
    std::cout << COLOR_YELLOW << "Closing server..." << COLOR_RESET << std::endl;
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
    std::cout << COLOR_GREEN << "Initializing server..." << COLOR_RESET << std::endl;
    _server.fd = socket(AF_INET, SOCK_STREAM, 0);
    if (_server.fd < 0)
        throw std::runtime_error(COLOR_RED "Error establishing the socket." COLOR_RESET);
    int setting = fcntl(_server.fd, F_GETFL, 0);
    fcntl(_server.fd, F_SETFL, setting | O_NONBLOCK);

    _server.addr.sin_family = AF_INET;
    if (_port < 1500)
        throw std::runtime_error(COLOR_RED "Invalid port number, should be > 1500" COLOR_RESET);
    _server.addr.sin_port = htons(_port);
    _server.addr.sin_addr.s_addr = htonl(INADDR_ANY);

    std::cout << COLOR_BLUE << "Binding server to port " << _port << "..." << COLOR_RESET << std::endl;
    if (bind(_server.fd, (struct sockaddr *) &_server.addr, sizeof(_server.addr)) < 0)
        throw std::runtime_error(COLOR_RED "Error binding." COLOR_RESET);

    std::cout << COLOR_CYAN << "Listening for connections..." << COLOR_RESET << std::endl;
    if (listen(_server.fd, SOMAXCONN) < 0)
        throw std::runtime_error(COLOR_RED "Error listening." COLOR_RESET);

    _pollfds = new std::vector<pollfd>(maxClients + 1);
    std::cout << COLOR_GREEN << "Server initialized successfully." << COLOR_RESET << std::endl;
}

void Server::socket_polling() {
    std::vector<pollfd> &connectionFds = *_pollfds;

    int num_ready = poll(connectionFds.data(), _num_clients + 1, -1);
    if (num_ready < 0)
        throw std::runtime_error(COLOR_RED "Error during polling." COLOR_RESET);
}

void Server::connect() {
    std::vector<pollfd> &connectionFds = *_pollfds;
    if (!(connectionFds[0].revents & POLLIN))
        return;

    socklen_t size = sizeof(_client.addr);
    _client.fd = accept(_server.fd, (struct sockaddr *) &_client.addr, &size);
    if (_client.fd == -1)
        throw std::runtime_error(COLOR_RED "Error accepting connection." COLOR_RESET);

    int setting = fcntl(_client.fd, F_GETFL, 0);
    fcntl(_client.fd, F_SETFL, setting | O_NONBLOCK);

    if (_num_clients == maxClients)
        throw std::runtime_error(COLOR_RED "Too many clients connected." COLOR_RESET);

    connectionFds[_num_clients + 1].fd = _client.fd;
    connectionFds[_num_clients + 1].events = POLLIN | POLLOUT;

    _num_clients++;
    std::cout << COLOR_YELLOW << "New client connected. Total clients: " << _num_clients << COLOR_RESET << std::endl;
}

void Server::read_client() {
    std::vector<pollfd> &connectionFds = *_pollfds;
    static std::string buffer;

    for (int i = 1; i <= _num_clients; i++) {
        if (connectionFds[i].fd != -1 && connectionFds[i].revents & POLLIN) {
            std::cout << COLOR_CYAN << "Reading from client..." << COLOR_RESET << std::endl;
            char buf[4096];
            memset(buf, 0, sizeof(buf));
            int bytes = recv(connectionFds[i].fd, buf, sizeof(buf), 0);
            if (bytes > 0) {
                std::string data(buf, bytes);
                std::cout << COLOR_MAGENTA << "Received: " << data << COLOR_RESET << std::endl;

                if (data.compare(0, 4, "PING") == 0) {
                    std::cout << COLOR_GREEN << "PING received, sending PONG..." << COLOR_RESET << std::endl;
                    std::string pingParameter = data.substr(5);
                    std::string pongMessage = "PONG :" + pingParameter + "\r\n";
                    send(connectionFds[i].fd, pongMessage.c_str(), pongMessage.size(), 0);
                }
            }
            if (bytes == -1) {
                if (errno == EWOULDBLOCK || errno == EAGAIN)
                    continue;
                else
                    throw std::runtime_error(COLOR_RED "Error reading data from client." COLOR_RESET);
            }
            if (bytes == 0)
                throw std::runtime_error(COLOR_RED "Client disconnected unexpectedly (bytes == 0)." COLOR_RESET);

            buffer += buf;
            execute(buffer);
        }
    }
}

void Server::launchServer() {
    std::vector<pollfd> &connectionFds = *_pollfds;
    connectionFds[0].fd = _server.fd;
    connectionFds[0].events = POLLIN;

    _num_clients = 0;

    std::cout << COLOR_GREEN << "Launching server..." << COLOR_RESET << std::endl;
    while (!isExit) {
        try {
            socket_polling();
            connect();
            send_message();
            read_client();
        } catch (std::exception &e) {
            std::cerr << COLOR_RED << "Error: " << e.what() << COLOR_RESET << std::endl;
            exit(EXIT_FAILURE);
        }
    }
}

void Server::send_message() {
    std::vector<pollfd> &connectionFds = *_pollfds;

    for (std::vector<s_message>::iterator it = _messages.begin(); it != _messages.end(); it++) {
        const s_message &message = *it;
        int index = -1;
        for (int i = 1; i <= _num_clients; i++) {
            if (connectionFds[i].fd == message.fd) {
                index = i;
                break;
            }
        }
        if (index == -1)
            throw std::runtime_error(COLOR_RED "Error sending message." COLOR_RESET);
    }
}

void Server::execute(std::string &message) {
    size_t pos = message.find("\r\n");
    if (pos == std::string::npos)
        return;
    std::string command = message.substr(0, pos);
    message.erase(0, pos + 2);
}
