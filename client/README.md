# CS50 Nuggets Game: Client Module
## Team Name: Lost at C


### Client
The Nuggets’ Client module is a program that interacts with the game server to provide functionality for playing the game. It initializes the game grid based on the map file passed in the command line. 


The program exits 0 if it runs successfully and nonzero if it encounters the following errors:
* error bad commandline (3)
* error bad hostname/port (4)
* error in message initialization (5)


### Usage
The *client* implemented in `client.c` implements the game logic and exports the following functions: 
```c
int main(const int argc, char* argv[]);
static void parseArgs(const int argc, char* argv[]);
static int playGame(addr_t server);
static bool handleInput(void* arg);
static bool handleMessage(void* arg, const addr_t from, const char* message)
static void gameOk(const char* message)
void gameQuit(const char* message, void* arg)
void gameError(const char* message)
void gameGold(const char* message, void* arg)
void gameGrid(const char* message, void* arg)
void gameDisplay(const char* message, void* arg)
static void setupWindow()
static void handleSize(char* nRows, char* nCols)


```
To use the client module, the user uses the following command line format:
```
./client hostname port [playername]
```
where _playername_ is an optional parameter that is passed if provided. If no name is provided, the user enters the game as a spectator.


### Implementation
To implement the server program, I used the following data structures: _game_ , _grid_, _nuggets_, _player_, and _spectator_. The details for each of these data structures can be found in the `README.md` file for the `structs` sub-directory. The server module also uses the `visibility` helper module that determines the view of the grid a player will receive based on their location and prior movement. The details of this helper module are also provided in the `README.md` in the `structs` subdirectory. 


The main `client.c` module consists of nine functions: `main`, `parseArg`, `playGame`, `handleInput`, `handleMessages`, `gameOk`,`gameQuit`, `gameError`, `gameGold`, `gameGrid`, `gameDisplay`, `handleSize`and `setupWindow`.
The `main` function calls `parseArgs` and other functions.


`parseArgs` takes the command line arguments as parameters, validates them, and assigns them to be passed through pointers if valid.


`playGame` is essentially the main driver of the client. It runs the message loop until the game is quit and will continuously parse received messages and send messages through the loop. It will also call print display to reprint the display.


`handleMessage` parses the incoming message to interpret the type of message and displays information onto the game window.


`handleInput` waits for keyboard input from the user and sends a corresponding message to the server.


`gameOk` gets the character information from the server and prints an OK message. It is called by handleMessage.


`gameQuit` quits the game and exits zero.


`gameError` prints the error message with a given explanation to the status line.


`gameGold` prints messages about the gold in the game currently, setting the cursor to print the message based on whether the user is a player or a spectator.


`gameGrid` Checks that the user window is large enough according to the message sent by server and adjusts the window size to fit the map


`gameDisplay` updates the string based on the message received from the server. It is  called by handleMessage, provided a message sent by the server. It prints the message to the window, copies the map, and moves the cursor accordingly. It operates based on the assumption that the message is not malformed.


`handleSize` Checks that the user window is big enough according to the rows and columns passed in its parameters. If the size is incompatible, prompts the user to make the window bigger until the map fits on the screen


`setupWindow` initializes the window and sets it up with newterm() and refreshes the screen




### Assumptions


There are no assumptions beyond what is give in the Requirements specifications. There are no known errors.




### Files
* `Makefile` - compilation procedure
* `client.c` - the implementation


### Compilation


To compile, simply type `make` in command line.




### Testing


To test, `make test` in `client` directory.
Run the command line arguments in client with the proper port name provided by the client.
To test with valgrind, simply `make valgrind` in the `client` directory.


