
OBJS = game.o grid.o nuggets.o player.o visibility.o spectator.o
LIB = structs.a -lm
L = ../libcs50

CFLAGS = -Wall -pedantic -std=c11 -ggdb $(FLAGS) -I$L -I../support
CC = gcc
MAKE = make

$(LIB): $(OBJS)
	ar cr $(LIB) $(OBJS)

# Dependencies: object files depend on header files
game.o: game.h $L/mem.h $L/file.h player.h nuggets.h grid.h spectator.h ../support/message.h
grid.o: grid.h $L/mem.h $L/file.h
nuggets.o: nuggets.h
player.o: player.h $L/mem.h
visibilty.o: visibility.h grid.h
spectator.o: spectator.h

.PHONY: clean

# clean up after our compilation
clean:
	rm -f core
	rm -f $(LIB) *~ *.o
