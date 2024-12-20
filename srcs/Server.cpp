#include "../includes/server.hpp"

// Constructeur par défaut
Server::Server()
{
	std::cout << GREEN << "-------------- SERVER --------------" << RESET << std::endl;
	this->sock_fd = -1;
	sock_opt = 1;
	poll_nb = 10;
	user_max = 10;
}

// Constructeur de copie
Server::Server(Server const &obj)
{
	*this = obj;
}

// Opérateur d'affectation
Server &Server::operator=(Server const &obj){
	if (this != &obj)
	{
		this->sock_fd = obj.sock_fd;
		this->port = obj.port;
		this->password = obj.password;
		this->sock_user = obj.sock_user;
		this->poll_fd = obj.poll_fd;
	}
	return *this;
}

// Destructeur
Server::~Server()
{
	std::cout << RED << "----------- SERVER CLOSED -----------" << RESET << std::endl;
}

// Récupère le descripteur de socket du serveur
int	Server::getSockFd()
{
	return (this->sock_fd);
}

// Récupère le numéro de port sur lequel le serveur écoute
int	Server::getPort()
{
	return (this->port);
}

// Récupère le descripteur de fichier d'un utilisateur par son pseudo
int	Server::getFdByNick(std::string nickname)
{
	int	fd;

	for (size_t i = 0; i < sock_user.size(); i++)
	{
		if (nickname == sock_user[i].getNickname())
		{
			fd = sock_user[i].getFduser();
			return (fd);
		}
	}
	return (-1);
}

// Récupère le mot de passe du serveur
std::string	Server::getPassword()
{
	return (this->password);
}

// Récupère un pointeur vers un utilisateur par son descripteur de fichier
User	*Server::getClientByFd(int fd)
{
	for (size_t i = 0; i < sock_user.size(); i++)
	{
		if (this->sock_user[i].getFduser() == fd)
			return (&this->sock_user[i]);
	}
	return (NULL);
}

// Récupère un pointeur vers un utilisateur par son pseudo
User	*Server::getClientByNickname(std::string nickname)
{
	for (size_t i = 0; i < sock_user.size(); i++)
	{
		if (this->sock_user[i].getNickname() == nickname)
			return (&this->sock_user[i]);
	}
	return (NULL);
}

// Récupère un pointeur vers un canal par son nom
Channel	*Server::getChannel(std::string name)
{
	for (size_t i = 0; i < this->channel.size(); i++)
	{
		if (this->channel[i].getChannelName() == name)
			return (&channel[i]);
	}
	return (NULL);
}

// Définit le descripteur de fichier du socket du serveur
void Server::setSockFd(int fd)
{
    this->sock_fd = fd;
}

// Définit le port sur lequel le serveur doit écouter les connexions entrantes
void Server::setPort(int port)
{
    this->port = port;
}

// Définit le mot de passe nécessaire pour se connecter au serveur
void Server::setPassword(std::string password)
{
    this->password = password;
}

// Ajoute un nouvel utilisateur à la liste des clients connectés au serveur
void Server::setNewUser(User newuser)
{
    this->sock_user.push_back(newuser);
}

// Ajoute un nouveau canal à la liste des canaux disponibles sur le serveur
void Server::setNewChannel(Channel newchannel)
{
    this->channel.push_back(newchannel);
}

// Ajoute une structure pollfd à la liste des descripteurs à surveiller
void Server::setPollfd(pollfd fd)
{
    this->poll_fd.push_back(fd);
}

// Supprime un utilisateur de la liste des clients en utilisant son descripteur de fichier
void Server::removeClient(int fd)
{
    for (size_t i = 0; i < this->sock_user.size(); i++)
    {
        if (this->sock_user[i].getFduser() == fd)
        {
            this->sock_user.erase(this->sock_user.begin() + i);
            return;
        }
    }
}

// Supprime un descripteur de fichier de la liste de surveillance poll_fd
void Server::removeFd(int fd)
{
    for (size_t i = 0; i < this->poll_fd.size(); i++)
    {
        if (this->poll_fd[i].fd == fd)
        {
            this->poll_fd.erase(this->poll_fd.begin() + i);
            return;
        }
    }
}

// Nettoie les canaux en retirant un utilisateur spécifique et en supprimant les canaux vides
void Server::clearChannel(int fd)
{
    int flag;
    std::string reply;

    for (size_t i = 0; i < this->channel.size(); i++)
    {
        flag = 0;
        if (channel[i].getUserByFd(fd))
        {
            channel[i].removeUserByFd(fd);
            flag = 1;
        }
        else if (channel[i].getOperatorByFd(fd))
        {
            channel[i].removeOperatorByFd(fd);
            flag = 1;
        }
        if (channel[i].getUserCount() == 0)
        {
            channel.erase(channel.begin() + i);
            i--;
            continue;
        }
        if (flag)
        {
            reply = ":" + getClientByFd(fd)->getNickname() + "!~" + getClientByFd(fd)->getUser() + "@localhost QUIT Quit\r\n";
            channel[i].broadcastMessage(reply);
        }
    }
}

// Envoie un message à un socket spécifique (fd)
void Server::notifyUsers(std::string message, int fd)
{
    std::cout << GREEN << RARROW << RESET << message;
    if (send(fd, message.c_str(), message.size(), 0) == -1)
        std::cerr << "send() failed" << std::endl;
}

// Formate et envoie un message avec un code d'erreur et des informations supplémentaires
void Server::notifyClient2(int errnum, std::string user, std::string channel, int fd, std::string message)
{
    std::stringstream ss;
    std::string rep;

    ss << ":localhost " << errnum << " " << user << " " << channel << message;
    rep = ss.str();
    if (send(fd, rep.c_str(), rep.size(), 0) == -1)
        std::cerr << "send() failed" << std::endl;
}

// Formate et envoie un message avec un code d'erreur et des informations supplémentaires, sans nom de canal
void Server::notifyClient3(int errnum, std::string user, int fd, std::string message)
{
    std::stringstream ss;
    std::string rep;

    ss << ":localhost " << errnum << " " << user << " " << message;
    rep = ss.str();
    if (send(fd, rep.c_str(), rep.size(), 0) == -1)
        std::cerr << "send() failed" << std::endl;
}

// Ferme tous les descripteurs de fichiers (sockets) ouverts pour les clients et pour le serveur lui-même
void Server::closeFd()
{
    for (size_t i = 0; i < sock_user.size(); i++)
    {
        std::cout << "FD[" << sock_fd << "] disconnected" << std::endl;
        close(sock_user[i].getFduser());
    }
    if (sock_fd != -1)
    {
        std::cout << "Server (FD[" << sock_fd << "]) disconnected" << std::endl;
        close(sock_fd);
    }
}

// Vérifie si un argument est un numéro de port valide
bool Server::isPortValid(std::string port)
{
    return (port.find_first_not_of("0123456789") == std::string::npos &&
            atoi(port.c_str()) >= 1024 && atoi(port.c_str()) <= 65535);
}

// Vérifie si un utilisateur est enregistré sur le serveur
bool Server::isRegistered(int fd)
{
    if (!getClientByFd(fd) || getClientByFd(fd)->getNickname().empty() || 
        getClientByFd(fd)->getUser().empty() || getClientByFd(fd)->getNickname() == "*" || 
        !getClientByFd(fd)->isConnected())
        return (false);
    return (true);
}

// Vérifie si un canal existe déjà sur le serveur
bool Server::isChannelAvailable(std::string channelName)
{
    if (channelName.empty() || channelName.size() <= 2 || channelName[0] != '#')
        return (false);
    
    std::string chan = channelName.substr(1);
    for (std::vector<Channel>::iterator it = channel.begin(); it != channel.end(); ++it)
    {
        if (it->getChannelName() == chan)
            return (true);
    }
    return (false);
}
