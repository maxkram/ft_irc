#include "../includes/channel.hpp"

// Constructeur par défaut de la classe Channel
Channel::Channel()
{
    this->channelName = "";
    this->topicName = "";
    this->created_at = "";
    this->topic = 0;
    this->key = 0;
    this->limit = 0;
    this->invitOnly = 0;
    this->topicRestriction = false;
    
    char mode[5] = {'i', 't', 'k', 'o', 'l'};
    for (int i = 0; i < 5; i++)
        channelMode.push_back(std::make_pair(mode[i], false));
}

// Constructeur de copie de la classe Channel
Channel::Channel(Channel const &src)
{
    *this = src;
}

// Destructeur de la classe Channel
Channel::~Channel()
{
}

// Opérateur d'affectation de la classe Channel
Channel &Channel::operator=(Channel const &rhs)
{
    if (this != &rhs)
    {
        this->channelName = rhs.channelName;
        this->topicName = rhs.topicName;
        this->password = rhs.password;
        this->topic = rhs.topic;
        this->key = rhs.key;
        this->limit = rhs.limit;
        this->sock_user = rhs.sock_user;
        this->admin = rhs.admin;
        this->invitOnly = rhs.invitOnly;
        this->topicRestriction = rhs.topicRestriction;
        this->created_at = rhs.created_at;
        this->channelMode = rhs.channelMode;
    }
    return (*this);
}

// Retourne le nom du canal
std::string Channel::getChannelName()
{
    return (channelName);
}

// Retourne le nom du sujet
std::string Channel::getTopicName()
{
    return (topicName);
}

// Retourne le mot de passe du canal
std::string Channel::getChannelPassword()
{
    return (this->password);
}

// Génère une liste des utilisateurs qui sont membres du canal et la retourne
std::string Channel::getUserList()
{
    std::string list;

    for (size_t i = 0; i < admin.size(); i++)
    {
        list += "@" + admin[i].getNickname();
        if ((i + 1) < admin.size())
            list += " ";
    }
    if (sock_user.size())
        list += " ";
    for (size_t i = 0; i < sock_user.size(); i++)
    {
        list += sock_user[i].getNickname();
        if ((i + 1) < sock_user.size())
            list += " ";
    }

    return (list);
}

// Retourne la date de création du canal
std::string Channel::getCreationDate()
{
    return (this->created_at);
}

// Retourne les modes du canal sous forme de chaîne de caractères
std::string Channel::getChannelModes()
{
    std::string mode;

    for (size_t i = 0; i < channelMode.size(); i++)
    {
        if (channelMode[i].first != 'o' && channelMode[i].second)
            mode.push_back(channelMode[i].first);
    }
    if (!mode.empty())
        mode.insert(mode.begin(), '+');

    return (mode);
}

// Retourne une référence à la liste des utilisateurs non-opérateurs du canal
std::vector<User> &Channel::getUsers()
{
    return (sock_user);
}

// Retourne une référence à la liste des opérateurs du canal
std::vector<User> &Channel::getOperators()
{
    return (admin);
}

// Retourne un pointeur vers l'utilisateur ayant le descripteur de fichier donné dans sock_user
User *Channel::getUserByFd(int fd)
{
    for (std::vector<User>::iterator it = sock_user.begin(); it != sock_user.end(); ++it)
    {
        if (it->getFduser() == fd)
            return &(*it);
    }
    return (NULL);
}

// Retourne un pointeur vers l'opérateur ayant le descripteur de fichier donné dans admin
User *Channel::getOperatorByFd(int fd)
{
    for (std::vector<User>::iterator it = admin.begin(); it != admin.end(); ++it)
    {
        if (it->getFduser() == fd)
            return &(*it);
    }
    return (NULL);
}

// Recherche un utilisateur par nom dans sock_user et admin
User *Channel::getFindUserByName(std::string name)
{
    for (std::vector<User>::iterator it = sock_user.begin(); it != sock_user.end(); ++it)
    {
        if (it->getNickname() == name)
            return &(*it);
    }
    for (std::vector<User>::iterator it = admin.begin(); it != admin.end(); ++it)
    {
        if (it->getNickname() == name)
            return &(*it);
    }
    return (NULL);
}

// Retourne le statut de l'invitation (0 ou 1)
int Channel::getInviteOnlyStatus()
{
    return this->invitOnly;
}

// Retourne le statut du sujet (0 ou 1)
int Channel::getTopicStatus()
{
    return this->topic;
}

// Retourne la clé du canal (mot de passe)
int Channel::getKeyStatus()
{
    return this->key;
}

// Retourne la limite du nombre d'utilisateurs
int Channel::getUserLimit()
{
    return this->limit;
}

// Retourne les restrictions sur le sujet (true ou false)
bool Channel::getTopicRestriction() const
{
    return (this->topicRestriction);
}

// Retourne l'option de mode du canal spécifiée par l'index i
bool Channel::getChannelModeOption(size_t i)
{
    return (channelMode[i].second);
}

// Retourne une liste de pointeurs vers les utilisateurs du canal
std::vector<User *> Channel::getUserPointers()
{
    std::vector<User *> pointers;
    for (size_t i = 0; i < sock_user.size(); i++)
    {
        pointers.push_back(&sock_user[i]);
    }
    return (pointers);
}

// Définit le nom du canal
void Channel::setChannelName(std::string name)
{
    this->channelName = name;
}

// Définit le nom du sujet
void Channel::setTopicName(std::string topic)
{
    this->topicName = topic;
}

// Définit le mot de passe du canal
void Channel::setChannelPassword(std::string password)
{
    this->password = password;
}

// Définit le statut d'invitation (0 ou 1)
void Channel::setInviteOnlyStatus(int invitOnly)
{
    this->invitOnly = invitOnly;
}

// Définit les restrictions sur le sujet (true ou false)
void Channel::setTopicRestriction(bool restriction)
{
    this->topicRestriction = restriction;
}

// Définit le statut du sujet (0 ou 1)
void Channel::setTopicStatus(int topic)
{
    this->topic = topic;
}

// Définit la clé du canal (mot de passe)
void Channel::setKeyStatus(int key)
{
    this->key = key;
}

// Définit la limite du nombre d'utilisateurs
void Channel::setUserLimit(int limit)
{
    this->limit = limit;
}

// Définit l'option de mode du canal spécifiée par l'index i
void Channel::setChannelMode(size_t i, bool mode)
{
    channelMode[i].second = mode;
}

// Définit la date de création du canal
void Channel::setCreationDate()
{
    std::time_t _time = std::time(NULL);
    std::ostringstream oss;
    oss << _time;
    this->created_at = std::string(oss.str());
}

// Retire un utilisateur du canal en fonction de son descripteur de fichier
void Channel::removeUserByFd(int fd)
{
    for (std::vector<User>::iterator it = sock_user.begin(); it != sock_user.end(); ++it)
    {
        if (it->getFduser() == fd)
        {
            sock_user.erase(it);
            break;
        }
    }
}

// Retire un opérateur du canal en fonction de son descripteur de fichier
void Channel::removeOperatorByFd(int fd)
{
    for (std::vector<User>::iterator it = admin.begin(); it != admin.end(); ++it)
    {
        if (it->getFduser() == fd)
        {
            admin.erase(it);
            break;
        }
    }
}

// Ajoute un utilisateur à la liste des membres du canal
void Channel::addUser(User user)
{
    sock_user.push_back(user);
}

// Ajoute un utilisateur à la liste des opérateurs du canal
void Channel::addOperatorOnChannel(User user)
{
    admin.push_back(user);
}

// Vérifie si le nom du canal est correct
void Channel::checkChannelName(std::string channelName)
{
    if (channelName.empty() || channelName.size() < 2)
        throw std::runtime_error("Incorrect channel name");
    if (channelName[0] != '&')
        throw std::runtime_error("Incorrect channel name, it must begin with &");
    for (size_t i = 0; i < channelName.size(); ++i)
    {
        if (channelName[i] == ' ' || channelName[i] == 0x07 || channelName[i] == ',')
            throw std::runtime_error("Incorrect channel name");
    }
}

// Retourne le nombre total d'utilisateurs dans le canal (opérateurs + utilisateurs normaux)
size_t Channel::getUserCount()
{
    size_t num = this->sock_user.size() + this->admin.size();
    return (num);
}

// Vérifie si un utilisateur avec le nom donné est présent dans le canal
bool Channel::isUserInChannel(std::string &name)
{
    for (size_t i = 0; i < sock_user.size(); i++)
    {
        if (sock_user[i].getNickname() == name)
            return true;
    }
    for (size_t i = 0; i < admin.size(); i++)
    {
        if (admin[i].getNickname() == name)
            return true;
    }
    return false;
}

// Vérifie si un utilisateur avec le nom donné est opérateur dans le canal
bool Channel::isUserOperator(std::string &name)
{
    for (std::vector<User>::iterator it = admin.begin(); it != admin.end(); ++it)
    {
        if (it->getNickname() == name)
            return (true);
    }
    return (false);
}

// Vérifie si le canal a des opérateurs
bool Channel::hasOperators()
{
    if (admin.empty())
        return (false);
    return (true);
}

// Envoie un message à tous les utilisateurs et opérateurs présents dans le canal
void Channel::broadcastMessage(std::string reply)
{
    for (size_t i = 0; i < admin.size(); i++)
    {
        if (send(admin[i].getFduser(), reply.c_str(), reply.size(), 0) == -1)
            std::cerr << "send() failed" << std::endl;
    }
    for (size_t i = 0; i < sock_user.size(); i++)
    {
        if (send(sock_user[i].getFduser(), reply.c_str(), reply.size(), 0) == -1)
            std::cerr << "send() failed" << std::endl;
    }
}

// Envoie un message à tous les utilisateurs et opérateurs présents dans le canal sauf un
void Channel::broadcastMessage2(std::string reply, int fd)
{
    for (size_t i = 0; i < admin.size(); i++)
    {
        if (admin[i].getFduser() != fd)
        {
            if (send(admin[i].getFduser(), reply.c_str(), reply.size(), 0) == -1)
                std::cerr << "send() failed" << std::endl;
        }
    }
    for (size_t i = 0; i < sock_user.size(); i++)
    {
        if (sock_user[i].getFduser() != fd)
        {
            if (send(sock_user[i].getFduser(), reply.c_str(), reply.size(), 0) == -1)
                std::cerr << "send() failed" << std::endl;
        }
    }
}

// Promeut le premier utilisateur non-opérateur au rang d'opérateur
void Channel::promoteFirstUserToOperator()
{
    admin.push_back(sock_user[0]);
    sock_user.erase(sock_user.begin());
}

// Promeut un utilisateur spécifique au rang d'opérateur
bool Channel::promoteUserToOperator(std::string& name)
{
    size_t i = 0;
    for (; i < sock_user.size(); i++)
    {
        if (sock_user[i].getNickname() == name)
            break;
    }
    if (i < sock_user.size())
    {
        admin.push_back(sock_user[i]);
        sock_user.erase(i + sock_user.begin());
        return true;
    }
    return false;
}

// Rétrograde un opérateur spécifique au rang d'utilisateur non-opérateur
bool Channel::demoteOperatorToUser(std::string& name)
{
    size_t i = 0;
    for (; i < admin.size(); i++)
    {
        if (admin[i].getNickname() == name)
            break;
    }
    if (i < admin.size())
    {
        sock_user.push_back(admin[i]);
        admin.erase(i + admin.begin());
        return true;
    }
    return false;
}

// Envoie un message au canal de la part d'un utilisateur
void Channel::notifyUsers(std::string msg, User &author)
{
    std::cout << "channel:: notifyUsers" << std::endl;
    (void)msg;
    (void)author;
}
