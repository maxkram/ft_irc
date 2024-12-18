#include "../headers/Server.hpp"

void printWelcomeBanner()
{
    std::cout << "\033[1;36m";
    std::cout << "  _  _     ____  	\n";
    std::cout << " | || |   |___ \\ \n";
    std::cout << " | || |_    __) |	\n";
    std::cout << " |__   _|  / __/ 	\n";
    std::cout << "    |_|   |_____|	\n";
	std::cout << "					\n";
    std::cout << "	Joyeux noel!	\n";
    std::cout << "\033[0m";
}

void	ft_irc(Server & server)
{
	try
	{
		server.initialize();
		server.launch();
	}
	catch (std::exception & e)
	{
		std::cerr << RED + "exception caught: " << e.what() + RESET << std::endl;
	}
}

int	main(int ac, char **av)
{
	int	port = 0;
	printWelcomeBanner();
	if (ac == 3)
	{
		port = atoi(av[1]);
		if (port < 1024 || port > 65535)
		{
			std::cout << RED + "Error Port : must be an int >= 1024 and =< 65535" + RESET << std::endl;
			return (1);
		}
		Server	server(port, av[2]);
		ft_irc(server);
	}
	else
	{
		std::cout << YELLOW + "right syntax is ./ircserv <port> <password>" + RESET << std::endl;
		return (1);
	}
	return (0);
}
