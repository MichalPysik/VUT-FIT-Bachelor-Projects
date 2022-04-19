CC=gcc
CFLAGS=-std=gnu99 -Wall -Wextra -Werror -pedantic -pthread

all: proj2 clean

proj2: proj2.o
	$(CC) $(CFLAGS) proj2.o -o proj2

proj2.o: proj2.c
	$(CC) $(CFLAGS) -c proj2.c

clean:
	rm -f proj2.o
