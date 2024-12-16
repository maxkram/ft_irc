#pragma once

# include "Client.hpp"
# include "messages.hpp"
# include <vector> 
# include <iostream>
# include <string>
# include <ctime>
# include <sys/socket.h> 

class	Channel
{
	public:

			Channel();
			~Channel();

			// Client Management
			void	addClient(Client * client);
			void	deleteClient(Client * client);
			bool	isChannelEmpty() const;
			// Operator Management
			void	addChannelOperator(Client * client);
			void	deleteChannelOperator(Client * client);
			bool	isChannelOperator(Client const & client);
			// Invitation Management
			void	addInvitedClient(Client * client);
			void	deleteInvitedClient(Client * Client);
			bool	isClientInvited(Client const & client) const;
			// Messaging
			void    sendChannelMessage(Client const & client, std::string const & cmd, std::string const & opt);
			// Topic Management
			std::string	const & getTopic();
			void 	setTopic(std::string const & val, Client & client);
			bool	isTopicEnabled();
			void	enableTopic(bool val);
			std::string getTopicAuthor();
			// Password Management
			bool 	hasPassword() const;
			std::string const & getPassword(void) const;
			void	setPassword(std::string const & str);
			// Channel Flags
			bool	isInviteEnabled();
			void 	setInviteEnabled(bool set);

			




	private:

			std::vector<Client *>	clients;
			std::vector<Client *>	chanOp;
			std::vector<Client *>	invited;
			bool		invite;
			bool		topicEnabled;
			bool		isPassword;
			std::string topicAuthor;
			std::string	topic;
			std::string	password;
};
