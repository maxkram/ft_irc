#include "../includes/server.hpp"

bool Server::signal = false;


void Server::initServer(int port, std::string pass)
{
    struct sockaddr_in addr;

    this->password = pass;
    this->port = port;

    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons(port);

    serverSocketFd = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocketFd < 0)
    {
        std::cerr << "Error: socket creation failed." << std::endl;
        exit(EXIT_FAILURE);
    }

    if (setsockopt(serverSocketFd, SOL_SOCKET, SO_REUSEADDR, (char *)&socketOptions, sizeof(socketOptions)) < 0)
    {
        std::cerr << "Error: setting socket options failed." << std::endl;
        exit(EXIT_FAILURE);
    }

    if (fcntl(serverSocketFd, F_SETFL, O_NONBLOCK) < 0)
    {
        std::cerr << "Error: setting socket to non-blocking failed." << std::endl;
        exit(EXIT_FAILURE);
    }

    if (bind(serverSocketFd, (const struct sockaddr*)&addr, sizeof(addr)) < 0)
    {
        std::cerr << "Error: binding socket failed." << std::endl;
        exit(EXIT_FAILURE);
    }

    if (listen(serverSocketFd, 3) < 0)
    {
        std::cerr << "Error: listening on socket failed." << std::endl;
        exit(EXIT_FAILURE);
    }

    std::cout << "Connect socket: FD[" << serverSocketFd << "]" << std::endl;

    newUserPollFd.fd = serverSocketFd;
    newUserPollFd.events = POLLIN;
    pollFds.push_back(newUserPollFd);
}

void Server::checkPollEvents()
{
    std::cout << "Waiting for a connection...\n";

    while (Server::signal == false)
    {
        status = poll(&pollFds[0], pollFds.size(), 5000);
        if (status < 0 && Server::signal == false)
            throw(std::runtime_error("poll() failed"));

        for (size_t i = 0; i < pollFds.size(); i++)
        {
            if (pollFds[i].revents & POLLIN)
            {
                if (pollFds[i].fd == serverSocketFd)
                    Server::acceptNewClient();
                else
                    Server::handleData(pollFds[i].fd);
            }
        }
    }

    closeFd();
}

void Server::acceptNewClient()
{
    User client;
    int cli_sock;
    struct sockaddr_in client_addr;
    socklen_t socklen = sizeof(client_addr);

    cli_sock = accept(serverSocketFd, (sockaddr *)&client_addr, &socklen);
    if (cli_sock == -1)
    {
        std::cerr << "Error: Server::acceptNewClient(): accept() failed." << std::endl;
        exit(EXIT_FAILURE);
    }

    if (fcntl(cli_sock, F_SETFL, O_NONBLOCK) == -1)
    {
        std::cerr << "Error: Server::acceptNewClient(): fcntl() failed." << std::endl;
        exit(EXIT_FAILURE);
    }

    newUserPollFd.fd = cli_sock;
    newUserPollFd.events = POLLIN | POLLOUT;
    newUserPollFd.revents = 0;

    client.setFduser(cli_sock);
    client.setIp(inet_ntoa(client_addr.sin_addr));

    connectedUsers.push_back(client);

    pollFds.push_back(newUserPollFd);

    std::cout << "FD[" << cli_sock << "] connected" << std::endl;
}

void Server::handleData(int fd)
{
    User *client;
    char buf[1024];
    int bytes;
    std::vector<std::string> command;

    client = getClientByFd(fd);
    memset(buf, 0, sizeof(buf));

    bytes = recv(fd, buf, sizeof(buf) - 1, 0);
    if (bytes <= 0)
    {
        std::cout << "FD[" << fd << "] disconnected" << std::endl;
        clearChannel(fd);
        removeClient(fd);
        removePollFd(fd);
        close(fd);
    }
    else
    {
        client->setBuffer(buf);

        if (client->getBuffer().find_first_of("\r\n") == std::string::npos)
            return;

        command = splitBuffer(client->getBuffer());
        for (size_t i = 0; i < command.size(); i++)
        {
            std::cout << RED << LARROW << RESET << command[i] << std::endl;
            this->executeCommand(command[i], fd);
        }

        if (getClientByFd(fd))
            getClientByFd(fd)->clearBuffer();
    }
}

void Server::handleSignal(int signum)
{
    (void)signum;
    std::cout << " signal received!" << std::endl;
    Server::signal = true;
}