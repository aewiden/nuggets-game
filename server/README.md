# CS50 Nuggets Game: Server Module
## Team Name: Lost at C

### Server
The Nuggets’ Server module is a program that implements all of the game’s logic. It initializes the game grid based on the map file passed in the command line. 

The program exits 0 if it runs successfully and non zero if it encounters the following errors:
* error creating port (1)
* incorrect number of args passed (2)
* error initializing grid from passed filename (3)
* error initializing game (4)
* error in messageLoop (5)

### Usage
The *server* implemented in `server.c` implements the game logic and exports the following functions: 
```c
static void parseArgs(const int argc, char* argv[]);
bool initializeGame(grid_t* grid) ;
bool handleMessage(void* arg, const addr_t from, const char* message);
void handleKeypress(void* arg);
bool movePlayer(player_t* player, const int xchange, const int ychange);
void checkNuggets(player_t* player, const int xchange, const int ychange);
void endGame();
void updateDisplay(player_t* player);
```
To use the server module, the user uses the following command line format:
```
./server map.txt [seed]
```
where _seed_ is an optional parameter that is passed to `srand(seed)` if provided. If no seed is provided, the server uses `srand(getpid())`;

### Implementation
To implement the server program, I used the following data structures: _game_ , _grid_, _nuggets_, _player_, and _spectator_. The details for each of these data structures can be found in the `README.md` file for the `structs` sub-directory. The server module also uses the `visibility` helper module that determines the view of the grid a player will receive based on their location and prior movement. The details of this helper module are also provided in the `README.md` in the `structs` subdirectory. 

The main `server.c` module consists of nine functions: `main`, `initializeGame`, `handleMessage`, `handleKeypress`, `movePlayer`, `checkNuggets`,`initCurses`, `endGame`, and `updateDisplay`.


`main` simply calls `parseArgs`,`log_init`, and `messages_init` to initialize the network and announce the port number. It then calls `message_loop` which continously loops, waiting for messages from the client which it will pass to `handleMessage`. It exits zero if it exectues without running into errors.

`parseArgs` takes two arguments that describe the number of command line arguments that were passed, as well as a `char*` array of these arguments. The main function of `parseArgs` is to validate the commandline arguments and to load a grid from the passed map text file. It exits nonzero if it runs into an issue such as too many/too little command line arguments or if the optional seed argument is not a valid number. If the function successfully loads a `grid_t` grid, it passes the grid to the function `initializeGame`. It checks that `initializeGame` returns true.


`initializeGame` takes the grid loaded in `parseArgs` and initializes the global `game` variable. It drops at most 30 and at least 10 piles of gold nuggets in randomly chosen empty spaces on the map, with each pile of nuggets having a randomized value. Each `nugget_t` structure that represents a pile of nuggets is added to a _nuggetArray_. The `game_t` game struct is passed this nugget array as well as the grid and number of piles. This function returns true if it runs without error and false otherwise.


`handleMessages` is a function passed to `message_loop` and is called everytime the server receives a message from the `client` module. This function determines whether the message was a `KEY`, `PLAY`, or `SPECTATE` message and handles each one appropriately. For `KEY` messages, it calls `handleKeypress`. For `PLAY` messages, it initializes a player (if there are not already MaxPlayers in the game) and adds that player to the game struct and then sends that player `GRID`, `GOLD`, and `DISPLAY` messages. For a `SPECTATE` message, the function initializes a new `spectator_t*` spectator and adds it to the game struct, kicking out a prior spectator if one already exists. The function then sends `GRID`, `GOLD`, and `DISPLAY` messages to the spectator.

`handleKeypress` appropriately calls `movePlayer` depending on the key that was pressed. After the player has moved, this function calls `updateDisplay` so that each client receives an updated version of the game state. This function also checks if all the gold has been collected and if so, it calls `endGame`

`movePlayer` takes a player and a proposed movement and checks if it is a valid movement (if the next spot on the map is a valid space and not a border). If it is a valid move, the function updates the player's position and updates the game state grid. This function also calls `checkNuggets` to determine if the proposed movement would allow the player to collect gold. 

`checkNuggets` determines if the location the player wants to move to contains a pile of gold. If it does, this function gets the value of that pile of gold nuggets and updates the game state grid and sends a `GOLD` message to all the clients to update them about the amount of gold left in the game. 

`endGame` creates a game summary that contains each player's character, name, and score, and sends it to each of the clients.

`updateDisplay` is called everytime there is a change in the position of a player and if a new player joins. It updates the game state grid to reflect the changes in player's positions as well as the changes in characters from a '*' to a '.' after a player has collected gold. It broadcasts this new game state grid to each of the clients to update their displays.


### Assumptions

There are no assumptions beyond what is give in the Requirements specifications. There are no known errors.


### Files
* `Makefile` - compilation procedure
* `server.c` - the implementation

### Compilation

To compile, simply type `make` in command line.


### Testing

To test, `make test` in `server` directory.
To test with valgrind, simply `make valgrind` in the `server` directory.

