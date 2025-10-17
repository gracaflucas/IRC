NAME = ircserv

SRC_DIR = src
SRC = $(SRC_DIR)/main.cpp  $(SRC_DIR)/Server.cpp $(SRC_DIR)/utils.cpp\
$(SRC_DIR)/Channel.cpp $(SRC_DIR)/Client.cpp $(SRC_DIR)/Commands/Join.cpp
OBJ = $(SRC:.cpp=.o)

CC = c++
FLAGS = -Wall -Wextra -Werror -std=c++98
CPPFLAGS = -I$(SRC_DIR)

# Color codes
GREEN = \033[0;32m
BLUE = \033[0;34m
RED = \033[0;31m
RESET = \033[0m

all: $(NAME)

$(NAME): $(OBJ)
	@$(CC) $(FLAGS) -o $(NAME) $(OBJ)
	@echo "$(GREEN)Compiled successfully!$(RESET)"

$(SRC_DIR)/%.o: $(SRC_DIR)/%.cpp
	@$(CC) $(FLAGS) $(CPPFLAGS) -c $< -o $@

clean:
	@rm -f $(OBJ)
	@echo "$(BLUE)Object files cleaned.$(RESET)"

fclean: clean
	@rm -f $(NAME)
	@echo "$(RED)Full clean done.$(RESET)"

re: fclean all

.PHONY: all clean fclean re