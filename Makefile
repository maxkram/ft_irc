# Project Name
NAME            =   ircserv

# Directories
SRC_DIR         =   srcs
INC_DIR         =   includes
BIN_DIR         =   $(SRC_DIR)/bin

# Files
SRCS            =   $(wildcard $(SRC_DIR)/**/*.cpp $(SRC_DIR)/*.cpp)
OBJS            =   $(SRCS:$(SRC_DIR)/%.cpp=$(BIN_DIR)/%.o)

# Compiler Settings
CC              =   c++
CPPFLAGS        =   -std=c++98 -Wall -Wextra -Werror -I$(INC_DIR)
RM              =   rm -rf

# Targets
.PHONY: all clean fclean re

# Default target
all: $(NAME)

# Build the binary
$(NAME): $(OBJS)
	$(CC) $(CPPFLAGS) $(OBJS) -o $(NAME)
	@echo "Build complete: $(NAME)"

# Compile source files into object files
$(BIN_DIR)/%.o: $(SRC_DIR)/%.cpp
	@mkdir -p $(@D)  # Create necessary directories for the object files
	$(CC) $(CPPFLAGS) -c $< -o $@

# Clean object files
clean:
	$(RM) $(BIN_DIR)
	@echo "Cleaned object files and directories."

# Clean all generated files
fclean: clean
	$(RM) $(NAME)
	@echo "Fully cleaned project."

# Rebuild
re: fclean all
