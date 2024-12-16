#pragma once

# include <vector>
# include <string>
# include <utility>

class	Client
{
	public:
			Client(int sd, char * IP, uint32_t port );
			~Client();
			// Socket Management
			int	getSocket() const;
			void setSocket(int sd);

			// Buffer Management
			std::string	& getBuffer();
			
			// IP and Port Accessors
			std::string	const & getIP() const;
			uint32_t	getPort() const;
			
			// User Info Accessors and Mutators
			std::string	const & getNick() const;
			void	setNick(std::string const & val);

			std::string	const & getPass() const;
			void	setPass(std::string const & val);

			std::string	const & getUsername() const;
			void	setUsername(std::string const & val);

			std::string	const & getMode() const;
			void	setMode(std::string const & val);

			std::string	const & getRealName() const;
			void	setRealName(std::string const & val);
			
			// Connection State Accessors and Mutators
			// std::string	const & getUnused() const;
			void	setUnused(std::string const & val);


			bool	isConnected() const;
			void	setConnected(bool val);

			bool	isNicked() const;
			void	setNicked(bool val);

			bool	isUsered() const;
			void	setUsered(bool val);

			bool	isRegistered() const;
			void	setRegistered(bool val);

			bool	isMarkedForDeletion() const;
			void	setMarkedForDeletion(bool val);

			bool	isOperator() const;
			void	setOperator(bool val);

			// Channel Management
			std::vector<std::string>	& getChannels();

			bool	isInChannel(std::string const & str) const ;
			void	removeChannel(std::string const & str);
			
			// Away Status
			std::pair<bool, std::string> getAwayStatus() const;
			void	setAwayStatus(std::pair<bool, std::string> val);
			

	private:
			// Data Members
			int	socket;
			std::string	IP;
			uint32_t port;
			std::string	buffer;
			std::string	nick;
			std::string	pass;
			std::string user;
			std::string mode;
			std::string unused;
			std::string realname;
			bool	connected;
			bool	nicked;
			bool	usered;
			bool	registered;
			bool	beDeleted;
			bool	Operator;
			std::vector<std::string>	channels;
			std::pair<bool, std::string> away;
};

