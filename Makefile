ifeq ($(HOSTTYPE),)
	HOSTTYPE := $(shell uname -m)_$(shell uname -s)
endif

NAME = libft_malloc_$(HOSTTYPE).so
NAME_LINK = libft_malloc.so

PATH_SRC = ./src/

PATH_INC = ./inc

INCLUDES = -I $(PATH_INC)

CC = cc

CFLAGS = -Wall -Wextra -Werror $(INCLUDES)
LDFLAGS = -shared

SRC =	bonus.c \
	block.c \
	free.c \
	libft.c \
	list.c \
	malloc.c \
	realloc.c \
	show_alloc_mem.c


OBJ = $(addprefix $(PATH_SRC), $(SRC:.c=.o))

#____________RULES_____________#

.PHONY: clean fclean re test

all: $(NAME)

$(NAME): $(OBJ)
	# make -C libft/
	# $(CC) $(OBJ) -o $(NAME) $(LIBS) $(LDFLAGS)
	$(CC) $(OBJ) -o $(NAME) $(LDFLAGS)
	ln -s $(NAME) $(NAME_LINK)

#____CLEAN____#

clean:
	rm -f $(OBJ)

#___FCLEAN___#

fclean: clean
	rm -f libft_malloc_*.so
	rm -f $(NAME_LINK)

#____RE____#

re: fclean all
