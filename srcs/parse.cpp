#include "../includes/server.hpp"

// Avance l'index jusqu'au prochain espace dans la chaîne de caractères
void goToNextSpace(std::string s, size_t *i)
{
    while (s[*i] != 0 && s[*i] != ' ')
    {
        ++(*i);
    }
}

// Sépare les paramètres d'une commande en trois parties maximum
int Server::splitParams(std::string params, std::string splitParams[3])
{
    size_t i = 0;
    size_t k = 0;
    for (int j = 0; i < params.length() && j < 3; j++)
    {
        k = i;
        i = params.find_first_of(" ", k);
        if (i == std::string::npos
            || (j >= 1 && params[k] == ':'))
        {
            splitParams[j] = params.substr(k);
            break;
        }
        else if (i < params.length()) 
        {
            splitParams[j] = params.substr(k, i - k);
        }
        i += (1 & (params[i] == ' '));
    }
    return (0);
}

// Analyse la syntaxe de la commande PRIVMSG et vérifie si les paramètres sont corrects
int Server::validatePrivmsgSyntax(std::string split_mess[3], std::string split_params[3])
{
    if (split_mess[2].empty())
        return (3);

    if (split_params[0].empty() || split_params[0].find_first_of(",") != std::string::npos)
        return (6);

    if (split_params[1].empty() || split_params[1][0] != ':')
        return (3);
    else if (split_params[1][0] != ':' || split_params[1].length() == 1)
        return (2);

    return (0);
}

// Sépare un message en trois parties : préfixe, commande, et paramètres
int Server::splitMessage(std::string message, std::string split_mess[3])
{
    std::string s_tmp;
    size_t i = 0;

    if (message[i] == ':')
    {
        s_tmp = message.substr(0, message.find_first_of(" "));
        split_mess[0] = s_tmp;
        goToNextSpace(message, &i);
        i += (1 & (message[i] == ' '));
        if (message[i] == 0)
            return (1);
    }
    else
    {
        split_mess[0] = std::string();
    }

    s_tmp = message.substr(i, message.find_first_of(" ", i) - i);
    split_mess[1] = s_tmp;
    goToNextSpace(message, &i);
    i += (1 & (message[i] == ' '));
    if (message[i] == 0)
        return (1);
    else
    {
        s_tmp = message.substr(i);
        split_mess[2].append(s_tmp);
    }
    return (0);
}

// Analyse le message et le divise en trois parties : préfixe, commande et paramètres
std::vector<std::string> Server::dissectMessage(std::string &message)
{
    std::vector<std::string> commandParams;
    std::string split_mess[3] = {std::string(), std::string(), std::string()};

    splitMessage(message, split_mess);

    for (size_t i = 0; i < 3; i++)
    {
        commandParams.push_back(split_mess[i]);
    }

    return commandParams;
}

// Sépare les paramètres d'un message en utilisant l'espace comme délimiteur
std::vector<std::string> Server::extractParams(std::string& message)
{
    std::vector<std::string> param;
    std::stringstream ss(message);
    std::string line;

    while (std::getline(ss, line, ' '))
    {
        param.push_back(line);
    }

    return param;
}

// Sépare le buff en lignes de commande en utilisant les délimiteurs de fin de ligne (\r\n)
std::vector<std::string> Server::splitBuffer(std::string buff)
{
    std::vector<std::string> commandParams;
    std::istringstream stm(buff);
    std::string line;
    size_t end;

    while (std::getline(stm, line))
    {
        end = line.find_first_of("\r\n");
        if (end != std::string::npos)
            line.erase(end);

        commandParams.push_back(line);
    }

    return commandParams;
}

// Analyse et exécute une commande IRC reçue par le serveur
void Server::processCommand(std::string &message, int fd)
{
    std::vector<std::string> commandParams;
    std::vector<std::string> commandParts;

    if (message.empty())
        return;

    commandParts = dissectMessage(message);

    commandParams = extractParams(message);

    size_t nonspace = message.find_first_not_of(" \t\v");
    if (nonspace != std::string::npos)
        message = message.substr(nonspace);

    if ((commandParts[1] == "PASS" || commandParts[1] == "pass") && commandParams.size())
        PASS(message, fd);
    else if ((commandParts[1] == "NICK" || commandParts[1] == "nick") && commandParams.size())
        NICK(message, fd);
    else if ((commandParts[1] == "USER" || commandParts[1] == "user") && commandParams.size())
        USER(message, fd);
    else if ((commandParts[1] == "QUIT" || commandParts[1] == "quit") && commandParams.size())
        QUIT(message, fd);
    else if ((commandParts[1] == "PING" || commandParts[1] == "ping") && commandParams.size())
        PING(message, fd);
    else if ((commandParts[1] == "CAP" || commandParts[1] == "cap") && commandParams.size())
        return;
    else if (isRegistered(fd) && commandParams.size())
    {
        if (commandParts[1] == "JOIN" || commandParts[1] == "join")
            JOIN(message, fd);
        else if (commandParts[1] == "INVITE" || commandParts[1] == "invite")
            INVITE(message, fd);
        else if (commandParts[1] == "WHO" || commandParts[1] == "who")
            return;
        else if (commandParts[1] == "WHOIS" || commandParts[1] == "whois")
            return;
        else if (commandParts[1] == "TOPIC" || commandParts[1] == "topic")
            TOPIC(message, fd);
        else if (commandParts[1] == "PART" || commandParts[1] == "part")
            PART(message, fd);
        else if (commandParts[1] == "KICK" || commandParts[1] == "kick")
            KICK(message, fd);
        else if ((commandParts[1] == "MODE" || commandParts[1] == "mode") && commandParams[1][0] == '#')
            MODE_CHANNEL(message, fd);
        else if ((commandParts[1] == "MODE" || commandParts[1] == "mode") && commandParams[1][0] != '#')
            return;
        else if (commandParts[1] == "PRIVMSG" || commandParts[1] == "privmsg")
            PRIVMSG(message, fd);
        else
            notifyUsers(ERR_UNKNOWNCOMMAND(getClientByFd(fd)->getNickname(), commandParts[1]), fd);
    }
    else if (!isRegistered(fd))
        notifyUsers(ERR_NOTREGISTERED(std::string("*")), fd);
}
