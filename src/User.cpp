#include "../inc/irc.hpp"

User::User() {}

User::User(int fd, int id) : _fd(fd), id(id), isOperator(false) {}

User::~User() {}

User::User(User const &src) {
    *this = src;
}

User &User::operator=(User const &src) {
    if (this != &src) {
        _fd = src._fd;
        id = src.id;
        isOperator = src.isOperator;
    }
    return *this;
}

int User::get_fd() const {
    return _fd;
}

int User::get_id() const {
    return id;
}

std::string User::get_nick() const {
    return _nick;
}

std::string User::get_name() const {
    return _name;
}

std::string User::get_pw() const {
    return _pw;
}


void User::splitMessage(int fd, Server &server, std::string buf) {
    std::stringstream ss(buf);
    std::string word;
    int count;

    if (buf[0] == ':')
        count = 0;
    else
        count = 1;
    while (ss >> word) {
        if (count == 0)
            _message._prefix = word;
        else if (count == 1)
            _message._command = word;
        else if (count == 2)
            _message._params = word;
        else
            _message._params += " " + word;
        count++;
    }
    parseMessage(server);
}

void User::parseMessage(Server &server) {
    std::string type[] = {"PASS", "NICK", "USER", "/JOIN", "/KICK", "/INVITE", "/CHANNEL", "CAP", "/PASS", "/NICK", "/USER", "PING", "/INVALID" };
    int count = 0;
    std::cout << "Command : " << _message._command << std::endl;
    for (int i = 0; i < 12; i++){
        if (_message._command.compare(type[i]) != 0)
            count++;
        else
            break;
    }
    std::cout << "count : " << count << std::endl;
    switch (count) {
        case 0:
            passwordCheck(server);
            break;
        case 1:
            command_nick(server, this->_message);
            break;
        case 2:
            command_user(server, this->_message);
            break;
        case 3:
            break;
        case 4:
            break;
        case 5:
            break;
        case 6:
            break;
        case 7:
            break;
        case 8:
            break;
        case 9:
            break;
        case 10:
            break;
        case 11:
            command_ping(server, this->_message);
        case 13:
            std::cout << "Error: invalid command" << std::endl;
            break;
    }
}

void User::passwordCheck(Server &server) {
    std::cout << "password function checked" << std::endl;

    if (_message._params.compare(server.get_password()) != 0) {
        send(_fd, ERR_PASSWDMISMATCH, 26, 0);
        server.set_exit_status(true);
        throw std::runtime_error("Error wrong password");
    }
    else
        std::cout << "password is correct" << std::endl;
}

bool User::command_nick(Server &server, s_message &message) {
    std::cout << "command_nick function checked" << std::endl;
    std::string new_nick = message._params;
    for (std::vector<User>::iterator it = server.get_clients().begin(); it != server.get_clients().end(); it++) {
        if (it->get_nick().compare(new_nick) == 0) {
            std::cout << "Error: nickname already taken" << std::endl;
            return false;
        }
    }
    _nick = new_nick;
    send(_fd, NICK(_nick, new_nick).c_str(), NICK(_nick, new_nick).size(), 0);
    return true;
}

bool User::command_user(Server &server, s_message &message) {
    std::cout << "command_user function checked" << std::endl;
    std::stringstream ss(message._params);
    std::string word;
    int count = 0;

    while (ss >> word) {
        if (count == 0)
            this->_name = word;
        else if (count == 1)
            this->_hostName = word;
        else if (count == 2)
            this->_serverName = word;
        else if (count == 3 and word[0] == ':')
            this->_realName = word;
        else if (count == 3 and word[0] != ':')
            break;
        else
            this->_realName += " " + word;
        count++;
    }
    if (count < 4) {
        send(_fd, ERR_NEEDMOREPARAMS(message._command).c_str(), ERR_NEEDMOREPARAMS(message._command).size() , 0);
        return false;
    }
    send(_fd, RPL_WELCOME(_nick, _name, _hostName).c_str(), RPL_WELCOME(_nick, _name, _hostName).size(), 0);
    return true;
}

void User::command_ping(Server &server, s_message &message) {
    std::cout << "command_ping function checked" << std::endl;
    if (_message._params.empty()) {
        send(_fd, ERR_NOORIGIN(_message._command).c_str(), ERR_NOORIGIN(_message._command).size(), 0);
        return;
    }
    else
        send(_fd, PONG(_message._params).c_str(), PONG(_message._params).size(), 0);
}