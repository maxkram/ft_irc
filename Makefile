# Project Information
NAME            =   ircserv

# Directories
INC             =   includes/
BIN             =   srcs/bin

# Files
SRCS            =   $(shell find srcs -type f -name "*.cpp")
INCS            =   $(shell find inc -type f -name "*.hpp")
OBJS            =   $(SRCS:srcs/%.cpp=srcs/bin/%.o)

# Compiler and Flags
CC              =   c++
RM              =   rm -rf
CPPFLAGS        =   -std=c++98 -Wall -Wextra -Werror

# Exotic Colors
BOLD_GREEN      =   \033[1;92m
BOLD_CYAN       =   \033[1;96m
BOLD_YELLOW     =   \033[1;93m
BOLD_RED        =   \033[1;91m
PURPLE          =   \033[1;95m
BLUE            =   \033[1;94m
RESET           =   \033[0m

# Targets
all:            $(NAME)

$(BIN):
	@mkdir -p $(BIN)
	@echo "$(BOLD_CYAN)🌟 Creating bin directory...$(RESET)"

$(NAME): $(BIN) $(OBJS)
	@$(CC) $(CPPFLAGS) $(OBJS) -o $(NAME)
	@echo "$(BOLD_GREEN)✅ Executable $(NAME) created successfully!$(RESET)"
	@echo "$(BOLD_CYAN)✨ Run the server using the following command:$(RESET)"
	@echo "$(BOLD_YELLOW)   ./$(NAME) 6667 mypassword$(RESET)"

clean:
	@echo "$(PURPLE)🧹 Cleaning object files and binaries...$(RESET)"
	@$(RM) $(OBJS)
	@$(RM) $(BIN)
	@echo "$(BOLD_GREEN)✔️  Clean completed!$(RESET)"

fclean: clean
	@echo "$(BOLD_RED)❌ Removing executable $(NAME)...$(RESET)"
	@$(RM) $(NAME)
	@echo "$(BOLD_GREEN)✔️  Full clean completed!$(RESET)"

re: fclean all

# Compilation Rules
srcs/bin/%.o: srcs/%.cpp
	@mkdir -p $(shell dirname $@)
	@echo "$(BLUE)🚀 Compiling $<...$(RESET)"
	@$(CC) $(CPPFLAGS) -c $< -o $@ -I$(INC)

# Help Target
help:
	@echo "$(BOLD_CYAN)📘 How to Use the Application:$(RESET)"
	@echo "$(BOLD_GREEN)1. Run the server using the following command:$(RESET)"
	@echo "$(BOLD_YELLOW)   ./$(NAME) <localhost> <port> <password>$(RESET)"
	@echo ""
	@echo "$(BOLD_GREEN)2. Arguments:$(RESET)"
	@echo "$(PURPLE)   <localhost>: The hostname or IP address to bind the server (e.g., 127.0.0.1).$(RESET)"
	@echo "$(PURPLE)   <port>:      The port number for the server to listen on (e.g., 6667).$(RESET)"
	@echo "$(PURPLE)   <password>:  The server password for authentication.$(RESET)"
	@echo ""
	@echo "$(BOLD_GREEN)Example:$(RESET)"
	@echo "$(BOLD_YELLOW)   ./$(NAME) 6667 mypassword$(RESET)"

# Phony Targets
.PHONY: all clean fclean re help
