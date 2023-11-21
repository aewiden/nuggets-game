# CS50 Nuggets Project
# Team name: Lost at C
## CS50 Winter 2023


### Nuggets
This repository contains the code for the CS50 "Nuggets" game, in which players explore a set of rooms and passageways in search of gold nuggets.
The rooms and passages are defined by a *map* loaded by the server at the start of the game.
The gold nuggets are randomly distributed in *piles* within the rooms.
Up to 26 players, and one spectator, may play a given game.
Each player is randomly dropped into a room when joining the game.
Players move about, collecting nuggets when they move onto a pile.
When all gold nuggets are collected, the game ends and a summary is printed.

Nuggets is a multiplayer game that includes a server module to keep track of the game state and a client module that allows a person to join the game as a _player_ or _spectator_.The object of the game is for the player(s) to collect all the gold nuggets that are randomly dropped in the game grid. When the player(s) collects all the gold, the game ends.


The program exits 0 if it runs successfully and non zero if it encounters the following errors:
Bad commandline
Bad hostname/port
Mal Formatted message

### Usage


The *nuggets* game implements a _server_ module to implement all the game logic through the following functions:


```c
static void parseArgs(const int argc, char* argv[])
bool initializeGame(grid_t* grid)
bool handleMessage(void* arg, const addr_t from, const char* message)
void handleKeypress(void* arg)
bool movePlayer(player_t* player, const int xchange, const int ychange)
void checkNuggets(player_t* player, const int xchange, const int ychange)
static void initCurses()
void endGame()
void updateDisplay(player_t* p)
```
It interfaces with the commandline as follows:

```c
./server map.txt [seed]
```

The *nuggets* game also implements a _client_ module that allows a human to join the game as either a _player_ or _spectator_. It implements the following functions:


```c
static int parseArgs(const int argc, char* argv[]);
static int playGame      (const addr_t serverAddress);
static void setupWindow(void);
static bool handleInput  (void* arg);
static bool handleMessage(void* arg, const addr_t from, const char* message);
static void handleSize(char* nRows, char* nCols);
static void gameOk(const char* message);
static void gameGrid(const char* message, void* arg);
static void gameGold(const char* message, void* arg);
static void gameDisplay(const char* message, void* arg);
static void gameQuit(const char* message, void *arg);
static void gameError(const char* message);
```

It interfaces with the commandline as follows:

```c
./client hostname port [playername]
```

A _player_ is a client that receives a limited view of the game grid and is able to move around the grid to collect gold. A _spectator_ is a client that views the entire grid, all the nuggets, and all the players in the game but cannot play the game themselves. 


To play the game, the `server` module must be run first to establish a port number which the client can connect to by passing the number as one of its command line arguments. The details of the specific command line arguments for client and server are provided in their respective `README.md` files and are not repeated here.  


### Implementation


We implemented the *Nuggets* game through two modules, server and client.


The server module uses the following data structures in order to update and keep track of the game’s state: _game_ , _grid_, _nuggets_, _player_, and _spectator_. The details for each of these data structures can be found in the `README.md` file for the `structs` sub-directory. The server module also uses the `visibility` helper module that determines the view of the grid a player will receive based on their location and prior movement. The details of this helper module are also provided in the `README.md` in the `structs` subdirectory. The details of the server’s functions and implementation are provided in the `README.md` in the `server` subdirectory and are not repeated here.


The client module uses the following data structures in order to keep track of the player and messages being sent to and received from the server: _localclient_ and _grid_. The details for each of these data structures can be found in the `README.md` file for the `structs` sub-directory. The details of the server’s functions and implementation are provided in the `README.md` in the `client` subdirectory and are not repeated here.




### Assumptions


No assumptions beyond those that are clear from the specifications are made in the Nuggets game.


### Files


More information on each component in the program is available in the `.h` files, as well as in the `DESIGN.md` and `IMPLEMENTATION.md` files.
* `Makefile` - compilation procedure
* `server.c` - the implementation of game logic
* `client.c` - the implementation of the player/spectator logic
* `game.h` - the interface
* `game.h` - the implementation
* `grid.h` - the interface
* `grid.c` - the implementation of the game grid 
* `player.h` - the interface
* `player.c` - the implementation of the game player structure
* `spectator.h` - the interface
* `spectator.c` - the implementation of the game spectator structure
* `visibility.h` - the interface
* `visibility.c` - the implementation of the game visibility capacity
* `nuggets.h` - the interface
* `nuggets.c` - the implementation of the gold piles, or nuggets, in the game


### Compilation


To compile, simply type `make` in command line.


### Testing


To test, `make test` in the `server` directory which will print out the port number that the client shall connect to. Run the `client` module with that port number and the appropriate hostname that the server is being run on. Pass a player name to join the game in play mode. Otherwise, join the game as a spectator. Collect all the gold. 
 
To test with valgrind, simply `make valgrind` in the server directory and play as detailed above.

