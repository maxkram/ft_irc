#pragma once
#include "irc.hpp"

struct s_socket {
    int fd;
    struct sockaddr_in addr;
};

class Server {
    public:
        Server(int port, std::string password);
        ~Server();
        Server(Server const & src);
        Server & operator=(Server const & src);
        void launchServer();
    private:
        int _port;
        int _num_clients;
        bool isExit;

        s_socket _server;
        s_socket _client;

        std::vector<pollfd> *_pollfds;

        std::string _name;
        std::string _password;

        void init();
        void socket_polling();
        void connect();
        void read_client();


        pollfd connectionFds[SOMAXCONN];

        const static int maxClients = SOMAXCONN;

};