# Makefile for 'server' module
# CS50 W23
# Lost at C

OBJS = server.o
LIBS = ../structs/structs.a ../support/support.a ../libcs50/libcs50.a -lncurses -lm 

CFLAGS = -Wall -pedantic -std=c11 -ggdb $(TESTING) -I../structs -I../support -I../libcs50 -fstack-protector
CC = gcc
MAKE = make

# for memory-leak tests
VALGRIND = valgrind --leak-check=full --show-leak-kinds=all

server: server.o $(LIBS)
	$(CC) $(CFLAGS) $^ -o $@

server.o: ../libcs50/file.h ../libcs50/mem.o ../structs/grid.o ../structs/player.o ../structs/nuggets.o ../structs/game.o ../structs/visibility.o ../structs/spectator.o ../support/message.o

.PHONY: test valgrind clean

test: server
	./server ../maps/small.txt

valgrind: server
	$(VALGRIND) ./server ../maps/small.txt
clean:
	rm -rf *.dSYM  # MacOS debugger info
	rm -f *~ *.o
	rm -f server
	rm -f core
