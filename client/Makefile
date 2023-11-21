# Makefile for 'querier' module
# CS50 W23
# Sierra Lee

OBJS = client.o
LIBS = ../structs/structs.a ../support/support.a ../libcs50/libcs50.a -lncurses -lm 

CFLAGS = -Wall -pedantic -std=c11 -ggdb $(TESTING) -I../structs -I../support -I../libcs50 -fstack-protector
CC = gcc
MAKE = make

# for memory-leak tests
VALGRIND = valgrind --leak-check=full --show-leak-kinds=all

client: client.o $(LIBS)
	$(CC) $(CFLAGS) $^ -o $@
client.o: ../libcs50/file.h ../libcs50/mem.o ../structs/grid.o ../structs/player.o ../structs/nuggets.o ../structs/game.o ../structs/visibility.o ../structs/spectator.o ../support/message.o

.PHONY: test valgrind clean

test: client
	./client ../maps/main.txt

valgrind: client
	$(VALGRIND) ./client 
	
clean:
	rm -rf *.dSYM  # MacOS debugger info
	rm -f *~ *.o
	rm -f client
	rm -f core
