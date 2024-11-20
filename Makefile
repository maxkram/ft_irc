SRC =	src/main.cpp \
		src/Server.cpp
		
HEADERS =	inc/irc.hpp \
			inc/Server.hpp

OBJS =	$(SRC:.cpp=.o)

CC = 	c++

FLAGS =	-Wall -Wextra -Werror -std=c++98

NAME =	ircserv

RM =	rm -f

%.o: %.cpp
	@$(CC) $(FLAGS) -c $< -o $@

all : $(NAME)

$(NAME) : $(OBJS) $(HEADERS)
	@$(CC) $(FLAGS) $(OBJS) -o $(NAME)
	@printf "Compiling done\n"

clean:
	@$(RM) $(OBJS)
	@printf "Remove .o files\n"

fclean: clean
	@$(RM) $(NAME)
	@printf "Remove executable\n"

re: fclean all

.PHONY: all clean fclean re