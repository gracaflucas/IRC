
NAME		= ircserv

SRC_DIR		= src
INC_DIR		= include
CMD_DIR		= $(SRC_DIR)/Commands

SRCS		= $(SRC_DIR)/main.cpp \
			  $(SRC_DIR)/Server.cpp \
			  $(SRC_DIR)/Client.cpp \
			  $(SRC_DIR)/Channel.cpp \
			  $(SRC_DIR)/utils.cpp \
			  $(CMD_DIR)/Join.cpp \
			  $(CMD_DIR)/Privmsg.cpp \
			  $(CMD_DIR)/Part.cpp \

OBJS		= $(SRCS:.cpp=.o)

CC			= c++
CFLAGS		= -Wall -Wextra -Werror -std=c++98
IFLAGS		= -I$(INC_DIR)

GREEN		= \033[0;32m
BLUE		= \033[0;34m
YELLOW		= \033[0;33m
RED			= \033[0;31m
CYAN		= \033[0;36m
RESET		= \033[0m

# **************************************************************************** #
#                                RULES                                         #
# **************************************************************************** #

all: header $(NAME)

header:
	@echo "$(CYAN)"
	@echo "╔════════════════════════════════════════════════════════════╗"
	@echo "║                    IRC SERVER BUILD                        ║"
	@echo "╚════════════════════════════════════════════════════════════╝"
	@echo "$(RESET)"

$(NAME): $(OBJS)
	@echo "$(YELLOW)Linking objects...$(RESET)"
	@$(CC) $(CFLAGS) $(OBJS) -o $(NAME)
	@echo "$(GREEN)✓ $(NAME) compiled successfully!$(RESET)"
	@echo "$(CYAN)Run: ./$(NAME) <port> <password>$(RESET)\n"

%.o: %.cpp
	@echo "$(BLUE)Compiling $(notdir $<)...$(RESET)"
	@$(CC) $(CFLAGS) $(IFLAGS) -c $< -o $@

clean:
	@echo "$(YELLOW)Cleaning object files...$(RESET)"
	@rm -f $(OBJS)
	@rm -f $(SRC_DIR)/*.o
	@rm -f $(CMD_DIR)/*.o
	@echo "$(GREEN)✓ Object files cleaned.$(RESET)"

fclean: clean
	@echo "$(YELLOW)Removing executable...$(RESET)"
	@rm -f $(NAME)
	@echo "$(GREEN)✓ Full clean done.$(RESET)"

re: fclean all