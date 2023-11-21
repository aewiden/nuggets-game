# CS50 Nuggets
## Implementation Spec
### Lost at C, W23


According to the [Requirements Spec](REQUIREMENTS.md), the Nuggets game requires two standalone programs: a client and a server.
Our design also includes a _struct_ library that includes all other data structures that we created to implement the Nuggets game.
We describe each program and module separately.
We do not describe the `support` library nor the modules that enable features that go beyond the spec.
We avoid repeating information that is provided in the requirements spec.


## Plan for division of labor


* Sierra - Server (main, handleMessage(), gameOver()), Player
* Jusung - Visibility, Client (main, handleMessage())
* Bradley - Grid, Server (main, handleInput(), initializeGame())
* Avery - Nuggets, Client (main, handleInput())


All involved in testing and documentation for their respective modules. Smaller modules will be tested with short .c files, longer modules (e.g. client, server) will be tested with longer .c files that include the smaller modules. The program will be tested altogether with a bash script.




## Client


### Data structures
The *client* module uses the `localclient_t`, `grid_t`, and `addr_t` data structures.
We created a `localclient_t` data structure in order to hold information for each of the players that join the client.


The localclient data structure keeps track of the player's ID, purse, gold remaining, the size of the window, whether the player is a spectator or not, and the port number. It implements the following function definitions:




### Definition of function prototypes
A function to parse the command-line arguments, initialize the game struct, initialize the message module, and (BEYOND SPEC) initialize analytics module.


The `main` function calls `parseArgs` and other functions.


```c
int main(int argc, char* argv[])
```


`parseArgs` takes the command line arguments as parameters, validates them, and assigns them to be passed through pointers if valid.


```c
static int parseArgs(const int argc, char* argv[]);
```


sets up a game window using the ncurse library with a given row and column size.


```c
static bool setupWindow(void);
```


`playGame` is essentially the main driver of the client. It runs the message loop until the game is quit and will continuously parse received messages and send messages through the loop. It will also call print display to reprint the display.


```c
static void playGame(const addr_t serverAddress);
```


`handleMessage` parses the incoming message to interpret the type of message and displays information onto the game window.


```c
static bool handleMessages(void* arg, const addr_t* from, const char* message);
```


`handleInput` waits for keyboard input from the user and sends a corresponding message to the server.


```c
static bool handleInput(void* arg);
```
### Detailed pseudo code


#### `main`:


```
	call parseArgs
	call playGame
```


#### `parseArgs`:


```
	validate command line arguments
	initialize message module
	get serverAddress


	print assigned port number
	if there are 4 arguments:
		send PLAY [playername]
	if there are 3 arguments
		Send SPECTATE
	else:
		print error message for incorrect number of arguments
		exit
```


#### `playGame`:


```
	while player hasn’t quit game:
		call messageLoop with NULL for handleInput and handleMessage
```


#### `handleMessage`:


```
	check message for NULL
	parses the message
	if first word of message == OK:
		initialize player struct with playerID
	if first word of message == GRID:
		initialize size of grid
		call setupWindow
	if first word of message == GOLD:
		if client is player:
			update players gold value
			update display message
		if client is spectator:
			update how much gold is left
	if first word of message == DISPLAY:
		call printDisplay to load display and update from message
	if first word of message == QUIT:
		end loop
	if first word of message == ERROR:
		print error message
```


#### `setupWindow`:


```
	initialize ncurses window (call initscr())
	set window to nRows nColumns
	call cbreak
```




## Visibility Module


The `visibility` module determines how much of the total map a player should see on their display based on their location.


### Data structures


#### `grid_t`


The grid data structure and its functions were described in the earlier section called _server_.


### Definition of function prototypes
`visibility.c` implements the following function definitions:
```c
bool is_visible(grid_t* grid, const int pc, const int pr, const int c, const int );
grid_t* return_visible(char* map, int width, int height, int x, int y);
void join_two_grid(grid_t* old, grid_t* merge, int width, int height);
```


### Detailed pseudo code
#### `is_visible`:
```
Find the difference in y and x
Calculate the slope and constant
If line is either vertical or horizontal
 Set the equation of the line appropriately
Loop over the columns from pc to c (non-inclusive)
 If is grid point
   If not room spot
     Return false
 Else check the floor and ceiling
   If neither point is room spot
     Return false
Loop over pr to r (non-inclusive)
 If is grid point
   If not room spot
     Return false
 Else check the floor and ceiling
   If neither point is room spot
     Return false
```
---
#### `return_visible`:
```
Load grid from string
Loop over grid width
 Loop over grid height
   if `is_visible` is false, change char in grid to ‘ ’
Return grid
```
---

#### `join_two_grid`:
```
Loop over grid width
 Loop over grid height
   Get char at coordinates from old and merge maps
   If old is ‘*’ and merge is not
     If merge is not ‘@’
       Change old char to ‘.’
     Else, change old char to ‘@’
   Else if old isalpha and merge !isalpha
     If merge is not ‘@’
       Change old char to ‘.’
     Else, change old char to ‘@’
   Else if merge is not ‘ ’
     Change old to merge
```




## Server


### Data structures


The *server* module uses the `player_t`, `nuggets_t`, `grid_t`, `arg_t`, and `game_t` data structures.
We created a `player_t` and `spectator_t` data structure in order to hold information for each of the clients that join the game.


The player data structure keeps track of the player's x,y location, their IP address, the amount of gold they've collected, their name, and their symbol. It implements the following function definitions:


```c
typedef struct player player_t;
int player_getX(player_t* player);
int player_getY(player_t* player);
int player_getPurse(player_t* player);
char* player_getName(player_t* player);


player_t* player_new(int x, int y, char* name, addr_t* address);
void player_updatePos(player_t* player, int x, int y);
void player_updatePurse(player_t* player, int nuggets);
```


The spectator data structure keeps track of the spectator's IP address
Spectator implements the following function definitions:
```c
typedef struct spectator spectator_t;
spectator_t* spectator_new(addr_t* address);
addr_t* spectator_geAddress(spectator_t* spectator);
void spectator_delete(spectator_t* spectator);
```

The `nuggets_t` data structure represents one pile of nuggets in the game. It keeps track of its x and y location on the map and its value. The server initializes nuggets and adds them to the grid. The `nuggets` struct implements the following functions:
```c
typedef struct nuggets nuggets_t;
nuggets_t* nuggets_init(int value, int x, int y);
void nuggets_delete(nuggets_t* nugget);
int nuggets_getX(nuggets_t* nugget);
int nuggets_getY(nuggets_t* nugget);
int nuggets_getValue(nuggets_t* nugget);
```

The `grid_t` data structure holds properties of the grid that the players are playing on, including its width, height, and a 2D array which represents the entire grid. it implements the following function definitions:
```c
grid_t* grid_new(char** array, int width, int height);
int grid_get_height(grid_t* grid);
int grid_get_width(grid_t* grid);
grid_t* grid_load(char* filename);
char** grid_get_array(grid_t* grid);
char grid_get_char_at(grid_t* grid, int x, int y);
void grid_change_char_at(grid_t* grid, int x, int y, char c);
char* grid_toString(grid_t* grid);
void grid_delete(grid_t* grid);


```


The *server* module also implements a data struct internally called `arg_t`. This data struct holds an `addr_t` address and a `char` key and is used when calling `handleKeypress` in order to pass more information to the function. It implements the following definitions:
```c
typedef struct arg arg_t;
arg_t* arg_new(addr_t* address, char key);
```

The `game_t` data structure is a global data structure that holds the state of the game at any moment, including an array of the active player(s), the spectator (if any), an array of all the nugget piles, the initial grid, the grid that holds the game state (players and nuggets), the gold left, the number of piles, and the number of players. It implements the following function definitions:

```c
typedef struct game game_t;
game_t* game_new();
void game_setGrid(game_t* game, grid_t* grid);
void game_setGoldLeft(game_t* game, int goldCount);
void game_setPileNum(game_t* game, int pileCount);
void game_setNuggetArray(game_t* game, nuggets_t* nuggetArray[GoldMaxNumPiles]);
void game_setSpectator(game_t* game, spectator_t* spectator);
void game_addPlayer(game_t* game, player_t* player) ;
spectator_t* game_getSpectator(game_t* game);
player_t* game_getPlayer(game_t* game, char playerChar);
player_t* game_getPlayerFromAddress(game_t* game, addr_t* address) ;
nuggets_t* game_getNuggetArray(game_t* game);
int game_getPlayerCount(game_t* game);
int game_getGoldLeft(game_t* game);
int game_getPileCount(game_t* game);
grid_t* game_getGameState(game_t* game);
grid_t* game_getInitialGrid(game_t* game);
players_t** game_getPlayers(game_t* game);
```


### Definition of function prototypes


#### `parseArgs`:
A function to parse the command-line arguments, initialize the game struct, and initialize the message module.


```c
static int parseArgs(const int argc, char* argv[]);
```
Pseudo code:
```
validate command line
if seed provided
verify it is a valid seed number
seed the random-number generator with that seed
Else
seed the random-number generator with getpid()
verify map file can be opened for reading
Load grid
Pass grid to initializeGame
Confirm game was initialized correctly
```
---
#### `handleMessage`:
A function that handles a message received from the client accordingly to whether it was a `PLAY`, `SPECTATE`, or `KEY` message.


```c
void handleMessage(void* arg, const addr_t from, const char* message)
```
Pseudo code:
```
Check if valid message
If message is KEY
initialize an arg_t arg and initialize its address and key
  	Call handleKeypress(arg);


Else if message is PLAY
	Check that there aren’t max players
	Find a valid random location on the grid 
	Initialize a player at those locations
	Add the player to the game struct
Send GRID message with nrows and ncols
   	Send GOLD message with amount of gold in grid
   	Send DISPLAY message to each client with updated grid and visibility
 
else if message is SPECTATE
if there already exists a spectator
send QUIT message to spectator and delete spectator
Initialize a new spectator
	Send GRID message with nrows and ncols
Send GOLD message with amount of gold in grid
   	Send DISPLAY message to spectator with full visibility
Return false to keep looping for messages
```
---
#### `handleKeypress`
```c
bool handleKeypress(void* arg)


```
A function that handles keystrokes sent to the server from a client.


Pseudocode:
```
if keystroke is valid
 check keystroke
   if key pressed was ‘Q’
     send QUIT message to client
   if key pressed corresponds to movement
call movePlayer
   	call updateDisplay
   if there is no gold remaining
     call endGame
     return true
```

---
#### `initializeGame`
A function that sets up the grid and drops the nuggets in random piles in the grid.
```c
bool initializeGame(grid_t* grid)
```

Pseudo code:
```
initialize the game struct
initialize nugget array
initialize x,y coordinates at 0,0
calculate min and max nugget pile values
loop until gold count is less than 25 (max gold amount per pile)
randomly generate a value for nugget pile
 	loop while x,y coordinates is not a '.' on the grid
   		randomly generate x between 0 and grid width
   		randomly generate y between 0 and grid height
 	change char from '.' to '*' on the grid
 	initialize a new nugget struct at x,y with nugget value
 	add nugget struct to nugget array
 	decrement gold count by nugget value
allocate all remaining gold to last pile at an x,y that contains a '.'
set pile number in game struct
set nugget array in game struct
set grid in game struct
return true if everything worked without error
otherwise, returns false
```
#### `movePlayer`
A function that updates the location of the player based on the key press.
```c
bool movePlayer(player_t* player, const int xchange, const int ychange)
```
Pseudo code:
```
call checkNuggets
if the character at the location the player wants to move is not a '.' or '#'
 return false
update the position of the player to the new x,y coordinates
change the character at the new x,y location to the player character
Change the character at the old x,y location back to the original (‘.’ or ‘#’)
return true
```


---
#### `checkNuggets`
A function that checks whether the new location that the player wants to move to contains a nugget pile.
```c
void check_nuggets(player_t* player, const int xchange, const int ychange)
```
Pseudo code:
```
if the character at the location that the player wishes to move is a '*'
find the corresponding nugget struct in the nugget array based on the x,y locations and get its value
 	update the player's purse
 	delete the nugget struct
 	set the index in the nugget array to NULL
	Send an updated GOLD message to all of the clients
```
---

#### `endGame`
A function that tells all the players and the spectator (if one exists) that the game has ended
```c
void endGame()
```

Pseudo code:
```
initialize the beginning of the message "QUIT GAME OVER:\n"
allocate memory for the entire game summary
combine these messages together
for each player in the player array
 	get player purse, character, and name
 	add to the summary
for each player in the player array
 	get the address for each player
 	send the summary to the player
if there is a spectator in the game
 	get the address of the spectator
 	send the game summary to the spectator
```

#### `updateDisplay`
A function that is called everytime there is a change to the game state. It updates gameState and creates a new display string that it sends to every client. 
```c
void updateDisplay(player_t* p)
```
Pseudocode:
```c
If there is a spectator
	Update the game state grid
	Create a DISPLAY message string
	Get the spectator address
	Send the message to the spectator
Loop through every player
	Get the player’s address
	Determine its visible grid based on the player’s location
	Convert that grid to a string
	Append that to a DISPLAY string message
	Send the message to the player
```
---
## Testing plan


### unit testing


The `grid`, `visibility`, and `game` modules will be tested with a `.c` testing program and/or a `testing.sh` script that will check for functionality in edge cases and when provided with faulty input. The `c` program will also be tested with `valgrind` to ensure the module is not responsible for any memory errors or leaks.


### integration testing


The server will be tested by sending a variety of messages from the provided `miniclient.c` program to verify that the server responds to these messages correctly and sends appropriate messages in return.
The client will be tested by first making sure `handleMessages()` is working as intended between the client and server by printing out the parsed message after it is handled. Also we will test whether `handleInputs()` are being relayed correctly by checking the player is moving accordingly to the keyboard presses. We will test whether `spectator` is running as intended by verifying the gold message with the server message and contains the same version of the grid as the server.




### system testing


The client and server will be tested together by initializing the server, joining from other devices connected to different `babylon` servers as players, having multiple people join as `spectator` to verify that the server will disconnect the previous `spectator` whenever a new one joins, then playing the game to ensure that visibility, quitting, joining mid-game, collecting nuggets, and end-of-game conditions work as expected.


---


## Limitations


None that we know of so far

