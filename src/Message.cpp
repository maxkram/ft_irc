#include "../inc/irc.hpp"

Message::Message(void)
{
    return;
}
Message::Message(std::string str) : _fullStr(str)
{
    return;
}

Message::~Message(void)
{
    return;
}

Message::Message(Message const &src)
{
    *this = src;
}

Message &Message::operator=(Message const & src) {
    if (this != &src) {
        _fullStr = src._fullStr;
        _prefix = src._prefix;
        _command = src._command;
        _params = src._params;
    }
    return *this;
}

void Message::splitMessage(int fd) {
    std::stringstream ss(_fullStr);
    std::string word;
    int count;

    if (_fullStr[0] == ':')
        count = 0;
    else
        count = 1;
    while (ss >> word) {
        if (count == 0)
            _prefix = word;
        else if (count == 1)
            _command = word;
        else if (count == 2)
            _params = word;
        else
            _params += " " + word;
        count++;
    }

    parseMessage();
}

void Message::parseMessage() {
    std::string type[] = {"PASS", "NICK", "USER", "/JOIN", "/KICK", "/INVITE", "/CHANNEL", "/CAP", "/PASS", "/NICK", "/USER", "/INVALID"};
    int count = 0;

    for (int i = 0; i < 8; i++) {
        if (_command.compare(type[i]) != 0)
            count++;
        else
            break;
    }
    std::cout << count << std::endl;
    switch (count) {
        case 0:
            break;
        case 1:
            break;
        case 2:
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
        default:
            throw std::runtime_error("Error: invalid command");
    }
};

void Message::pass() {
};