#pragma once

#include "socket.hpp"
#include "irc.hpp"

class Binding : public Socket {
    private:
        int binding;
        void ConnectToNetwork(int sock, struct sockaddr_in address);
        
    public:
        Binding(int domain, int service, int protocol, int port, u_long interface);
        int getBinding();
};