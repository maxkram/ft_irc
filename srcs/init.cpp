#include "../includes/server.hpp"

// Déclaration et initialisation de la variable statique "signal" à false
bool Server::signal = false;


void Server::initServer(int port, std::string pass)
{
    struct sockaddr_in addr;

    this->password = pass;
    this->port = port;

    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons(port);

    sock_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (sock_fd < 0)
    {
        std::cerr << "Error: socket creation failed." << std::endl;
        exit(EXIT_FAILURE);
    }

    if (setsockopt(sock_fd, SOL_SOCKET, SO_REUSEADDR, (char *)&sock_opt, sizeof(sock_opt)) < 0)
    {
        std::cerr << "Error: setting socket options failed." << std::endl;
        exit(EXIT_FAILURE);
    }

    if (fcntl(sock_fd, F_SETFL, O_NONBLOCK) < 0)
    {
        std::cerr << "Error: setting socket to non-blocking failed." << std::endl;
        exit(EXIT_FAILURE);
    }

    if (bind(sock_fd, (const struct sockaddr*)&addr, sizeof(addr)) < 0)
    {
        std::cerr << "Error: binding socket failed." << std::endl;
        exit(EXIT_FAILURE);
    }

    if (listen(sock_fd, 3) < 0)
    {
        std::cerr << "Error: listening on socket failed." << std::endl;
        exit(EXIT_FAILURE);
    }

    std::cout << "Connect socket: FD[" << sock_fd << "]" << std::endl;

    new_user.fd = sock_fd;
    new_user.events = POLLIN;
    poll_fd.push_back(new_user);
}

void Server::checkPollEvents()
{
    std::cout << "Waiting for a connection...\n";

    while (Server::signal == false)
    {
        status = poll(&poll_fd[0], poll_fd.size(), 5000);
        if (status < 0 && Server::signal == false)
            throw(std::runtime_error("poll() failed"));

        for (size_t i = 0; i < poll_fd.size(); i++)
        {
            if (poll_fd[i].revents & POLLIN)
            {
                if (poll_fd[i].fd == sock_fd)
                    Server::acceptNewClient();
                else
                    Server::handleData(poll_fd[i].fd);
            }
        }
    }

    closeFd();
}

// Traite les connexions entrantes sur le socket du serveur (sock_fd)
// et crée un nouveau socket dédié pour chaque connexion acceptée
void Server::acceptNewClient()
{
    User client;
    int cli_sock;
    struct sockaddr_in client_addr;
    socklen_t socklen = sizeof(client_addr);

    cli_sock = accept(sock_fd, (sockaddr *)&client_addr, &socklen);
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

    new_user.fd = cli_sock;
    new_user.events = POLLIN | POLLOUT;
    new_user.revents = 0;

    client.setFduser(cli_sock);
    client.setIp(inet_ntoa(client_addr.sin_addr));

    sock_user.push_back(client);

    poll_fd.push_back(new_user);

    std::cout << "FD[" << cli_sock << "] connected" << std::endl;
}

// Lire les données provenant d'un socket et traiter les données lues en fonction du socket expéditeur
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
        removeFd(fd);
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
            this->processCommand(command[i], fd);
        }

        if (getClientByFd(fd))
            getClientByFd(fd)->clearBuffer();
    }
}

// Recevoir les signaux Ctrl+C et Ctrl+'\'
void Server::handleSignal(int signum)
{
    (void)signum;
    std::cout << " signal received!" << std::endl;
    Server::signal = true;
}
