# **************************************************************************** #
#                                                                              #
#                                                         ::::::::             #
#    Makefile                                           :+:    :+:             #
#                                                      +:+                     #
#    By: svan-has <svan-has@student.codam.nl>         +#+                      #
#                                                    +#+                       #
#    Created: 2023/01/31 19:05:22 by svan-has      #+#    #+#                  #
#    Updated: 2023/06/15 16:42:17 by svan-has      ########   odam.nl          #
#                                                                              #
# **************************************************************************** #

NAME := minishell
CC := cc
CFLAGS := -Werror -Wall -Wextra
LIBFT	:= ./library/libft
#MINISHELL	:= ./minishell.a
HEADERS := -I $(LIBFT)/includes -I includes
LIB	:= $(LIBFT)/libft.a
SRC_DIR := source
OBJ_DIR := object
SRC := \
		main.c\
		execution.c\

SRC := $(SRC:%=$(SRC_DIR)/%)
OBJ := $(SRC:$(SRC_DIR)/%.c=$(OBJ_DIR)/%.o)
DIR_DUP = mkdir -p $(@D)
RM := rm -rf

all: libft $(NAME)

libft:
	@$(MAKE) -C $(LIBFT)

$(NAME): $(OBJ)
	@$(CC) $(CFLAGS) -lreadline $(OBJ) $(LIB) $(HEADERS) -o $(NAME)
	$(info minishell Compiled)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	@$(DIR_DUP)
	@$(CC) $(CFLAGS) -c -o $@ $^ $(HEADERS)

clean:
	@$(RM) $(OBJ_DIR)
	@$(MAKE) -C $(LIBFT) clean

fclean: clean
	@$(RM) $(NAME)
	@$(MAKE) -C $(LIBFT) fclean

re: fclean all

.PHONY: all libft clean fclean