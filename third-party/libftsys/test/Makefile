CC = clang
CFLAGS = -g3 -I../include -nostdlib -ffreestanding -O0 -fPIE
LDFLAGS = -nostdlib -nostartfiles -ffreestanding -fPIE

all: test

../libftsys.a:
	make -C ..

test: test.o ../libftsys.a
	$(CC) test.o ../libftsys.a -o test $(LDFLAGS)

test.o: test.c
	$(CC) $(CFLAGS) -c test.c -o test.o

clean:
	rm -f test.o

fclean: clean
	rm -f test

re: fclean all

.PHONY: all clean fclean re
