#include "../includes/user.hpp"

// Constructeur par défaut de la classe User
User::User()
{
    this->fdUser = -1;
    this->nickname = "";
    this->user = "";
    this->ip = "";
    this->buff = "";
    this->realname = "";
    this->registered = false;
    this->connected = false;
    this->isOp = false;
    this->chanFounder = false;
    time(&this->created);
    char modes[2] = {'i', 'o'};
    for (int i = 0; i < 2; i++)
        userModes.push_back(std::make_pair(modes[i], false));
}

// Constructeur avec paramètres de la classe User
User::User(int fd, std::string nickname, std::string user)
    : fdUser(fd), nickname(nickname), user(user) {}

// Constructeur de copie de la classe User
User::User(User const &obj)
{
    *this = obj;
}

// Opérateur d'affectation de la classe User
User &User::operator=(User const &obj)
{
    if (this != &obj)
    {
        this->fdUser = obj.fdUser;
        this->nickname = obj.nickname;
        this->user = obj.user;
        this->ip = obj.ip;
        this->buff = obj.buff;
        this->registered = obj.registered;
        this->connected = obj.connected;
        this->isOp = obj.isOp;
        this->userModes = obj.userModes;
        this->realname = obj.realname;
        this->chanFounder = obj.chanFounder;
        this->created = obj.created;
    }
    return *this;
}

// Compare les pseudonymes de deux utilisateurs pour déterminer s'ils sont égaux
bool User::operator==(User const &rhs)
{
    if (this->nickname == rhs.nickname)
        return (true);
    return (false);
}

// Destructeur de la classe User
User::~User()
{}

// Retourne le descripteur de fichier de l'utilisateur
int User::getFduser()
{
    return (this->fdUser);
}

// Retourne une référence au pseudonyme de l'utilisateur
std::string &User::getNickname()
{
    return (this->nickname);
}

// Retourne le nom d'utilisateur (ou une chaîne vide s'il est vide)
std::string User::getUser()
{
    return (user.empty() ? std::string("") : user);
}

// Retourne l'adresse IP de l'utilisateur
std::string User::getIp()
{
    return (ip);
}

// Retourne le buff contenant les données reçues de l'utilisateur
std::string User::getBuffer()
{
    return (buff);
}

// Retourne le nom d'hôte au format "nickname!user"
std::string User::getHostname()
{
    std::string hostname = getNickname() + "!" + getUser();
    return (hostname);
}

// Retourne le nom réel de l'utilisateur
std::string User::getRealname() const
{
    return (this->realname);
}

// Retourne les modes de l'utilisateur sous forme de chaîne de caractères
std::string User::getUserModes()
{
    std::string mode;
    for (size_t i = 0; i < userModes.size(); i++)
    {
        if (userModes[i].first != 'o' && userModes[i].second)
            mode.push_back(userModes[i].first);
    }
    if (!mode.empty())
        mode.insert(mode.begin(), '+');
    return (mode);
}

// Retourne true si l'utilisateur est enregistré
bool User::isRegistered()
{
    return (registered);
}

// Retourne true si l'utilisateur est connecté
bool User::isConnected()
{
    return (this->connected);
}

// Retourne true si l'utilisateur est opérateur
bool User::isOperator()
{
    return (this->isOp);
}

// Vérifie si l'utilisateur est invité à un canal spécifique
bool User::isInvited(std::string &channel)
{
    for (size_t i = 0; i < this->invitation.size(); i++)
    {
        if (this->invitation[i] == channel)
            return (true);
    }
    return (false);
}

// Retourne l'état d'une option de mode utilisateur spécifique
bool User::getUserModeOption(size_t i)
{
    return (userModes[i].second);
}

// Retourne true si l'utilisateur est le fondateur du canal
bool User::isChannelFounder()
{
    return (this->chanFounder);
}

// Retourne l'heure de création de l'utilisateur
time_t User::getCreationTime() const
{
    return (created);
}

// Définit le descripteur de fichier de l'utilisateur
void User::setFduser(int fd)
{
    this->fdUser = fd;
}

// Définit le pseudonyme de l'utilisateur
void User::setNickname(std::string &nickname)
{
    this->nickname = nickname;
}

// Définit le nom d'utilisateur
void User::setUser(std::string &user)
{
    this->user = user;
}

// Définit l'adresse IP de l'utilisateur
void User::setIp(std::string ip)
{
    this->ip = ip;
}

// Ajoute les données reçues au buff de l'utilisateur
void User::setBuffer(std::string recv)
{
    buff += recv;
}

// Définit si l'utilisateur est enregistré
void User::setRegistered(bool val)
{
    registered = val;
}

// Définit si l'utilisateur est connecté
void User::setConnected(bool val)
{
    this->connected = val;
}

// Définit si l'utilisateur est opérateur
void User::setOperator(bool op)
{
    this->isOp = op;
}

// Définit si l'utilisateur est le fondateur du canal
void User::setChannelFounder(bool founder)
{
    this->chanFounder = founder;
}


// Définit le mode utilisateur pour une option spécifique
void User::setUserMode(size_t i, bool mode)
{
    userModes[i].second = mode;
}

// Définit le nom réel de l'utilisateur
void User::setRealname(std::string realname)
{
    this->realname = realname;
}

// Supprime le buff de l'utilisateur
void User::clearBuffer()
{
    buff.clear();
}

// Ajoute une invitation pour un canal spécifique
void User::addInvitation(std::string &channel)
{
    this->invitation.push_back(channel);
}

// Retire l'invitation quand l'utilisateur rejoint le canal
void User::removeInvitation(std::string &channel)
{
    for (size_t i = 0; i < this->invitation.size(); i++)
    {
        if (this->invitation[i] == channel)
        {
            this->invitation.erase(this->invitation.begin() + i);
            return;
        }
    }
}

