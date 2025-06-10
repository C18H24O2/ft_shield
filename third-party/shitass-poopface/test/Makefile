NAME = test-obf
SRC = test.c main.c
OBJ = $(SRC:.c=.o)

CC = clang
LIBSP = ../libshitass-poopface.so
CFLAGS = -fpass-plugin=$(LIBSP)
LDFLAGS =

all: 
	make -C ..
	make fclean $(NAME)

$(NAME): $(OBJ)
	$(CC) $(LDFLAGS) -o $@ $^
	strip -xXs $@

%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<

clean:
	rm -f $(OBJ)

fclean: clean
	rm -f $(NAME)

re: fclean all

.PHONY: all clean fclean re
