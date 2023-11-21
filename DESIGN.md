# CS50 Nuggets
## Design Spec
### Lost at C, Winter, 2023 



According to the [Requirements Spec](REQUIREMENTS.md), the Nuggets game requires two standalone programs: a client and a server.
Our design also includes grid, nuggets, and player modules.
We describe each program and module separately.
We do not describe the `support` library.
We avoid repeating information that is provided in the requirements spec.


## Client


The *client* acts in one of two modes:


1. *spectator*, the passive spectator mode described in the requirements spec.
2. *player*, the interactive game-playing mode described in the requirements spec.


### User interface


The user interfaces first through the command line where the input is in the form:
``` 
./server map.txt [seed]
```
Assuming success, the server module will print out its port number. A client can join the game with the following syntax in a separate terminal window:
```
./client hostname port [playername]
```
The _playername_ is optional but if provided, the client will be in player mode. Otherwise, the client joins the game in spectator mode. 


In player mode, the user interfaces with the program through keystrokes (specifics provided in the Requirements specifications).


In spectator mode, the client does not interface with the program, but rather watches as a player, or players, move around the map.


In player mode, the user interfaces with the program through keystrokes (specifics provided in the Requirements specifications).

In spectator mode, the client does not interface with the program, but rather watches as a player, or players, move around the map.
For more details, see the requirements spec for both the command-line and interactive UI.




### Inputs and outputs
User input includes the following keystrokes, the details of which are provided in the requirement specifications:


The output display will depend on the client mode. In spectator mode, the entire grid is displayed, showing all the gold to be collected and all the players. In player mode, only parts of the map and the contained information (i.e. nuggets and players) are displayed, according to visibility. It updates as the player(s) gathers the nuggets and as the player moves around the map, changing the visibility of the player.


For error handling, useful messages are printed to stderr. Some errors include too many command line arguments, invalid map.txt argument, failure to initialize `message`, and failure to connect to port.


### Functional decomposition into modules
main() : parses arguments, initializes server, handles game logic 
handleMessage() : update data structures based on messages received from server
handleInput() : send keystrokes to server


### Pseudo code for logic/algorithmic flow


The client will run as follows:
Validate that either 3 or 4 arguments were passed in the command line
Verify hostname (or IP address) and port number and determine player or spectator mode
Initialize display
Initialize network and join game with a `PLAY` or `SPECTATE` message to server, accordingly
Receive `GRID`, `GOLD`, and `DISPLAY` messages from server
Calls handleMessage() to appropriately react to each of these messages
If player mode
		Wait for input
Call handleInput which sends keystrokes to the server
If spectator mode
		Display entire grid
		Wait for `GOLD` and `DISPLAY` messages and continuously update the display as players move and collect gold. 
	Calls handleMessage() to parse message and update display accordingly


#### handleMessage function:
Everytime a message is received from the server, this function is called to handle the message. The details of each received message (`QUIT`, `OK`, `GRID`,  `GOLD`,  `DISPLAY`,  and `ERROR`) are explained further in the requirements specifications.


#### handleInput function:
Takes user keystroke input and sends it to the server module.


### Major data structures:

localclient data structure:
The localclient data structure keeps track of the player’s characteristics such as playerID, purse, and the amount of gold remaining. It also keeps track of the size of the window, whether the player is a spectator, and the port number.

---


## Server
### User interface


See the requirements spec for the command-line interface.
There is no interaction with the user.


### Inputs and outputs
The server takes a `map.txt` input that refers to an existing map document in the map directory. If the map file does not exist, the server sends an error message to the log file. After the server program initializes the network, it prints the hostname and port number to the terminal for the client(s) to connect to. 


If a new spectator attempts to join the game while there is already a spectator, the server sends a message telling the current spectator to quit, and then the server forgets the current spectator. 


Everytime the player moves or gathers nuggets, the server sends a message to the client to update the state of the game.


When the player collects all of the gold, the server sends a `QUIT` message to all the clients and prints out a summary of the game to stdout.


### Functional decomposition
initializeGame(): initialize the data structures, network, and print the port number
handleMessage(): parses the messages and appropriate deals with their contents
handleKeypress(): handles each key press and moves player, if possible
movePlayer(): moves player by specified amounts, if possible
checkNuggets(): checks if moving player by specified amounts will result in them collecting a nugget, sends GOLD messages to all players and spectator if it will 
updateDisplay(): sends DISPLAY messages to all clients with adjusted visibility for players
endGame(): informs all players that the game has ended and frees memory


### Pseudo code for logic/algorithmic flow


The server will run as follows:


   execute from a command line per the requirement spec
   parse the command line, validate parameters
   call initializeGame() to set up data structures
   initialize the 'message' module
   print the port number on which we wait
   call message_loop(), to await clients
   call handleMessage() and pass received message
   call endGame() to inform all clients the game has ended
   clean up


#### handleMessage()
This function is called every time the server receives a message. The grid_t* grid is passed to this module in addition to the message. If the message is a `PLAY` or `SPECTATE` message, the module responds appropriately and sends the grid to the client with full visibility. If the message is a `KEY` message, the function calls `handleKeypress`


#### handleKeypress();
If the key pressed is q or Q, send a QUIT message to the player; if the keypress indicates movement, call movePlayer and call updateDisplay if the move is valid. After the keypress has been handled, call endGame if all gold has been collected.


#### endGame():
This function is called to inform all the players that the game has ended.


#### initializeGame():
This function starts the game by setting up all the data structures, starting the network, and printing out the port number.


#### updateDisplay():
This function is called every time a player moves or joins the game. It updates the game state grid to reflect movements and then broadcasts the new game state to every player and the spectator if there is one. 


#### movePlayer():
This function checks whether or not the desired movement is a valid move (the next character is a ‘.’, ‘*’, or ‘#’). If it is a good move, the function updates the player’s position and changes the character at the new location to the player’s character. Likewise, it gets the old character from the initialGrid in the game state and replaces the player’s old location with the appropriate period or hashtag symbol.


#### checkNuggets():
​​If the character currently occupying the coordinates that the player will be moved into is a nugget, find that nugget in the nugget array, send a GOLD message to
all players and the spectator, and delete the nugget.


### Major data structures


Grid data structure:
One major data structure is the grid struct, implemented in `grid.c`. This struct contains the map format that the game will be played in. More information about this data structure is provided below in the _grid module_ section.


Player data structure:
Every player that joins the game has a corresponding `player` struct that contains the player’s current x,y coordinates, number of nuggets in their purse, and a character pointer to their name. More information about this data structure can be found below under the _player module_ section.


Nuggets data structure:
The nuggets data structure keeps track of the amount of gold in each nugget pile, as well as each pile’s location. It contains three integers – x coordinate, y coordinate, and value of gold. More information about this data structure is provided below in the _nuggets module_ section.


Game data structure:
The data for the ongoing game is stored in the `game` structure, which contains a `spectator`, two `grids`, and array of `players`, and integers for the player count, amount of gold left, and number of gold piles. The `game` module implements various functions to assign and retrieve these values at different points in the game’s progression.


---


## Grid module
The grid provides a structure that stores the height and width of the grid as integers, and stores the state of the grid as an array of strings. It contains functions for initializing a grid from a file, retrieving a grid’s height, width, and array of strings, retrieving the state of a grid in a single string, and freeing a grid’s memory. 


### Functional decomposition
Getter methods:
* `grid_get_array` takes a `grid_t*` as an argument and returns the array of its characters
* `grid_get_height` takes a `grid_t*` as an argument and returns its height
* `grid_get_width` takes a `grid_t*` as an argument and returns its width
* `grid_get_char_at` takes a `grid_t*`, int x, and int y as arguments and returns the player’s name
* `grid_toString` takes a `grid_t*` and returns the contents of the grid’s array in one string


Other methods:
* `grid_load` takes a string filename as parameters and initializes a `grid_t*` structure from the according grid passed. 
* ‘grid_new` creates and returns a new `grid_t*` from a given array, width, and height
* `grid_change_char_at` takes a `grid_t*`, int x, int y, and char c as arguments and changes the char at the specified location to the passed char c
* `grid_delete` frees all memory associated with the passed grid


visibility methods:
* `is_visible` takes grid_t* grid, const int pc, const int pr, const int c, const int r as parameters and returns a boolean on whether the specific coordinate is visible to the player or not
* `return_visible` takes char* msg, int width, int height, int x, and int y as parameters, loads the grid from the string and height and width given, and runs the `is_visible` method and changes the string at the locations between ‘@’ and ‘ ’ depending on their visibility.
* `join_two_grind` takes grid_t* old, grid_t* merge, int width, int height to interchange between the visible grid and the non-visible grid as the player moves.


---


### Player module
The player module is a data structure that contains information about each player that enters the game. Namely, the structure holds the player’s x and y coordinates, the amount of gold the player has collected, as well as the player’s name.


Everytime a client enters the game in player mode, a new player struct is initialized.


### Functional decomposition
Getter methods:
* `player_getX` takes a `player_t*` as an argument and returns its x location
* `player_getY` takes a `player_t*` as an argument and returns its y location
* `player_getPurse` takes a `player_t*` as an argument and returns its _purse_ value or the amount of gold the player has collected
* `player_getName` takes a `player_t*` as an argument and returns the player’s name


Other methods:
* `player_new` takes int x, int y, and a string name as parameters and initializes a new `player_t*` object with these x and y values and name string. The player’s purse is initialized to 0.
* `player_updatePos` takes a `player_t*` player and an int x and y and updates the passed player’s location to the x and y coordinates passed. 
* `player_updatePurse` takes a `player_t*` player and an integer number of nuggets and adds the number of nuggets to the player’s current value stored in its purse.


---


## Nuggets module


The nuggets module is a data structure that contains information about each nugget pile within the game. Namely, the structure holds the nugget’s x and y coordinates and the amount of gold in the nugget pile.


Everytime a grid is loaded, nugget piles are initialized.


### Functional decomposition


Getter methods:
* `nuggets_getX` takes a `nuggets_t*` as an argument and returns its x location
* `nuggets_getY` takes a `nuggets_t*` as an argument and returns its y location
* `nuggets_getValue` takes a `nuggets_t*` as an argument and returns the amount of gold in the pile

Other methods:
* `nuggets_init` takes int x, int y, and an int value as parameters and initializes a new `nuggets_t*` object with these x and y values and gold value.

## Game module

## Game module


Game data structure:
The data for the ongoing game is stored in the `game` structure, which contains a `spectator`, two `grids`, and array of `players`, and integers for the player count, amount of gold left, and number of gold piles. The `game` module implements various functions to assign and retrieve these values at different points in the game’s progression.


### Functional decomposition
* `game_new` : Create a new game structure
* `game_setGrid` : Sets the initial grid and game state grid. The initial grid only holds the nugget piles whereas the game state grid updates as players join and collect gold
* `game_setGoldLeft` : Sets the goldLeft of the passed game to the passed goldCount
* `game_setPileNum` sets the passed game's numPiles to the passed `pileCount`
* `game_setNuggetArray` sets the game's nuggetArray to the passed `nuggetArray`
* `game_setSpectator` Sets the passed game's `spectator` to the passed `spectator_t*`
* `game_addPlayer` : Adds the passed player struct to the game's array of players and updates the game's player count
* `game_getSpectator` : Returns a pointer to the game's spectator 
* `game_getPlayer: Return a player from a passed character
* `game_getPlayerFromAddress` : Return a pointer to the player struct corresponding to the passed address
* `game_getNuggetArray` : Return the game's array of nugget pointers 
* game_getPlayerCount` : Return the number of players active in the game
* `game_getGoldLeft`: Return the amount of gold left in the game
* `game_getPileCount`: Return the number of piles left in the game
* `game_getGameState` returns the `grid_t*` that represents the game's initial state
* `game_getInitialGrid` returns the `grid_t*` that represents the game's initial state
* `game_getPlayers` returns the array of pointers to players in the game
* ‘game_delete` deletes the game structure


—--


## Testing Plan
Individual testing for grid, game, and visibility
Test server and client with invalid command line arguments
Tested server with miniclient.c
Tested client and server together

