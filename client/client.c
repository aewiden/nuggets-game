/*
 * client.c - implementation of client module 
 *
 * CS50 W23 Nuggets Project
 * team name: Lost at C
 */
 
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <stdbool.h>
#include <ncurses.h>
#include <unistd.h>
#include <math.h>

#include "mem.h"
#include "file.h"
#include "grid.h"
#include "player.h"
#include "nuggets.h" 
#include "game.h"
#include "visibility.h"
#include "spectator.h"
#include "message.h"
#include "log.h"

/**************** local types ****************/
typedef struct localclient {
  // player's characteristics
  char playerID[2];
  int purse;
  int left;

  // the size of the window
  char nRows[5];
  char nCols[5];
  bool isSpectator;
  int portNumber;

} localclient_t;


localclient_t currentClient;

/**************** file-local functions ****************/
static int parseArgs(const int argc, char* argv[]);

// handling communications
static int playGame      (const addr_t serverAddress);
static void setupWindow(void);
static bool handleInput  (void* arg);
static bool handleMessage(void* arg, const addr_t from, const char* message);
static void handleSize(char* nRows, char* nCols);

// handling specific communication
static void gameOk(const char* message);
static void gameGrid(const char* message, void* arg);
static void gameGold(const char* message, void* arg);
static void gameDisplay(const char* message, void* arg);
static void gameQuit(const char* message, void *arg);
static void gameError(const char* message);


/*************** main() ****************/
/* 
 * Caller provides: 
 *   arg from standard input
 * We do: 
 *   Call parseArgs and playGame
 *   Return nothing
 * We assume: 
 *   The message is not malformed (is of form ERROR [explanation])
 */
int main(const int argc, char* argv[]) {
    int play;
    // if cannot initialize message
    if (message_init(NULL) == 0) {
        return 5; 
    }
    int checkArgs;
    // check arguments with parseArgs
    if ((checkArgs = parseArgs(argc, argv)) != 0) {
        exit(checkArgs);
    } else{
        // set up host and port
        const char* serverHost = argv[1];
        const char* serverPort = argv[2];
        addr_t server; // address of the server
        message_setAddr(serverHost, serverPort, &server);
        play = playGame(server);
    }
    return play;
}

/**************** parseArgs ****************/
/* 
 * takes the command line arguments as parameters, validates them, and 
 * making connection to server according to the command line arguments (player vs spectator)
 * Caller provides: 
 *   Arg from standard input 
 * We do: 
 *   Return an integer to identify arguments error.
 * We assume: 
 *   The message is not malformed (is of form ERROR [explanation])
 */
static int parseArgs(const int argc, char* argv[]){

  // check arguments
  const char* program = argv[0];
  if (argc < 1 || argc > 4) {
    fprintf(stderr, "usage: %s hostname port\n", program);
    fprintf(stderr, "incorrect number of commandline arguments %d \n", argc);
    return 3; // bad commandline
  }

  // get the address from the host and port
  const char* serverHost = argv[1];
  const char* serverPort = argv[2];
  currentClient.portNumber = atoi(argv[2]);
  addr_t server; // address of the server
  // check if can set message address
  if (!message_setAddr(serverHost, serverPort, &server)) {
    fprintf(stderr, "can't form address from %s %s\n", serverHost, serverPort);
    return 4; // bad hostname/port
  }

  if (argc == 4){
    char* playerName = argv[3];

    // play message
    char* message = (char *) malloc(7 + strlen(playerName) + 1); // 7 for approximation of playerName length
    strcpy(message, "PLAY ");
    strcat(message, playerName);
    currentClient.isSpectator = false;
    // set up ncurses window
    setupWindow();
    fprintf(stderr, "Player mode; name '%s' \n", playerName);
    message_send(server, message);
    free(message);
  }

  // send "PLAY [playername] for player client"
  else if (argc == 3){

    // specatate message
    char* message = (char *) malloc(10);
    strcpy(message, "SPECTATE");
    currentClient.isSpectator = true;
    // set up ncurses window
    setupWindow();
    message_send(server, message);
    free(message);
  }

  // return 0 for no fatal commandline arguments
  return 0;
}

/**************** playGame() ****************/
/* 
 * Acts as a wrapper to call message_loop
 * Return an integer to identify game error.
 * Caller provides: 
 *   Server address 
 * We do: 
 *   Return an integer to identify game error.
 * We assume: 
 *   The message is not malformed (is of form ERROR [explanation])
*/
static int playGame(addr_t server){

  bool play = message_loop(&server, 0, NULL, handleInput, handleMessage);
  message_done();
  // status code based on loop
  return play? 0 : 1;
}




/**************** handleInput() ****************/
/* 
 * takes the key input from stdin and combine it with a "KEY " to send to server
 * Caller provides: 
 *   keystrokes from standard input 
 * We do: 
 *   Return true if the message loop should exit, otherwise false
 * We assume: 
 *   The message is not malformed (is of form ERROR [explanation])
 */
static bool handleInput(void* arg) {
    // ensure the arg is a valid address
    addr_t* serverp = arg;
    if (serverp == NULL) {
        fprintf(stderr, "handleInput called with arg=NULL");
        return true;
    }
    if (!message_isAddr(*serverp)) {
        fprintf(stderr, "handleInput called without a correspondent.");
        return true;
    }

    // continuous loop for a window char input
    char c;
    if ((c = getch()) != 'q') {    // read one character
        // package input and send to server
        char* packagedMessage = (char *) malloc(6);
        sprintf(packagedMessage, "KEY %c", c);
        message_send(*serverp, packagedMessage);
        free(packagedMessage);
    
        // normal case: keep looping
        return false;
    } else{

        // if the client presses q, exits preemptively
        endwin();
		return true;
	}
}

/**************** handleMessage() ****************/
/*  
 * Take a message from the server, parses it accordingly, validate them, and handle specific to the command type.
 * Return an integer to identify game error.
 * Caller provides: 
 *   server address, message
 * We do: 
 *   Return true if the message loop should exit, otherwise false.
 * We assume: 
 *   The message is not malformed (is of form ERROR [explanation])
 */
static bool handleMessage(void* arg, const addr_t from, const char* message) {
    if(message == NULL) {
        fprintf(stderr, "ERROR message is invalid \n");
        return false;
    }

    if(strncmp(message, "OK ", strlen("OK ")) == 0) {
        gameOk(message);
    } else if(strncmp(message, "QUIT ", strlen("QUIT ")) == 0) {
        gameQuit(message, arg);
    } else if(strncmp(message, "ERROR ", strlen("ERROR ")) == 0) {
        gameError(message);
    } else if(strncmp(message, "DISPLAY\n", strlen("DISPLAY\n")) == 0) {
        gameDisplay(message, arg);
    } else if(strncmp(message, "GRID ", strlen("GRID ")) == 0) { 
        gameGrid(message, arg);
    } else if(strncmp(message, "GOLD ", strlen("GOLD ")) == 0) { 
        gameGold(message, arg);
    } else {
        printf("Invalid message.\n");
        refresh();
    }
    return false;
}

/**************** gameOk() ****************/
/*
 * Gets the character information from the server and prints OK message. Called by handleMessage
 * Caller provides: 
 *   Message 
 * We do: 
 *   Print OK message
 *   Print player's information
 * We assume: 
 *   The message is not malformed (is of form OK )
 */
static void gameOk(const char* message){
    // parsing out charID and setting that to the current client's ID
    char charID[2];
    sscanf(message, "OK %s", charID);
    fprintf(stderr, "%s \n", charID);
    // if the charID is not null, print the ok message and the player/char ID
    if (charID != NULL){
        fprintf(stderr, "OK message received: OK %s \n", charID);
        sprintf(currentClient.playerID, "%s", charID);
    }
}

/**************** gameQuit() ****************/
/*
 * Quits the game. Called by handleMessage
 * Caller provides: 
 *   Message 
 * We do: 
 *   Exit nCurses and end the game
 *   prints the explanation
 * We assume: 
 *   The message is not malformed (is of form QUIT [explanation])
 */
void gameQuit(const char* message, void* arg) {
    // end the game
    nocbreak();
    endwin();
    // copy the message and allocate memory
    char* messageCopy = malloc(strlen(message) + 1);
    strcpy(messageCopy, message);
    // copy the message and add to the error message
    char* quitFunc = messageCopy + strlen("QUIT ");
    // print and refresh, exit 0
    fprintf(stdout, "%s", quitFunc);
    free(messageCopy);
    exit(0);
}

/**************** gameError() ****************/
/*
 * Prints error messages after status line
 * Caller provides: 
 *   Message 
 * We do: 
 *   Print error explation
 * We assume: 
 *   The message is not malformed (is of form ERROR [explanation])
 */
void gameError(const char* message) {
    // copy the message and allocate memory
    char* messageCopy = malloc(strlen(message) + 1);
    strcpy(messageCopy, message);
    // copy the message and add to the error message
    char* errorFunc = messageCopy + strlen("ERROR ");
    // print and refresh
    fprintf(stderr, "%s", errorFunc);
    refresh();

    free(messageCopy);
}


/**************** gameGold() ****************/
/*
 * Prints messages about the gold in the game currently. Called by handleMessage
 * Caller provides: 
 *   Message (such as that sent by server)
 *   localclient struct for this client
 * We do: 
 *   Print message about the player's gold
 * We assume: 
 *   The message is not malformed (is of form GOLD nuggets, purse, left)
 */
void gameGold(const char* message, void* arg) {
    int nuggets, purse, left;
    // get information from message
    sscanf(message, "GOLD %d %d %d", &nuggets, &purse, &left);
    if (nuggets >= 0 && purse >= 0 && left >= 0){
        // print gold message and set client's purse and remaining
        fprintf(stderr, "GOLD message received: GOLD %d %d %d \n", nuggets, purse, left);
        currentClient.purse = purse;
        currentClient.left = left;

        if (currentClient.playerID != NULL){
        // if the client is a spectator, set cursor and print
            if (currentClient.isSpectator) {
                move(0,0);
                char message[68];
                // print spectate message
                sprintf(message, "Spectator: %d nuggets unclaimed. Play at plank %d\n", currentClient.left, currentClient.portNumber);
                addstr(message);
                refresh();
            }
            else {
                if (nuggets == 0) {          // not the player that just collected the nuggets
                    // reset cursor and print gold message
                    move(0, 0);
                    char message[68] ;
                    sprintf(message, "Player %s has %d nuggets (%d nuggets unclaimed).\n", currentClient.playerID, currentClient.purse, currentClient.left);
                    addstr(message);
                    refresh();
                } else { 
                    // reset cursor and print gold message
                    move(0, 0);
                    char message[68];
                    sprintf(message, "Player %s has %d nuggets (%d nuggets unclaimed). GOLD received: %d\n", currentClient.playerID, purse, left, nuggets);
                    addstr(message);
                    refresh();
                }
            }
            
        }
    }
}

/**************** gameGrid() ****************/
/*
 * Ensures that the user window is large enough according to the message sent by server. Called by handleMessage
 * Caller provides: 
 *   Grid message
 * We do: 
 *   adjust the window size to fit the map, call handleSize
 * We assume: 
 *   The message is not malformed
 */
void gameGrid(const char* message, void* arg) {
    int rows;
    int cols;
    // get the number of rows and columns from the grid message
    sscanf(message, "GRID %d %d", &rows, &cols);
    // print the rows and cols
    sprintf(currentClient.nRows, "%d", rows);
    sprintf(currentClient.nCols, "%d", cols);
    // if the rows and cols are not 0
    if (rows >0 && cols >0){
      // adjust window's size to fit map
      fprintf(stderr, "GRID message received: GRID %s %s \n", currentClient.nRows, currentClient.nCols);
      // call method to handle size
      handleSize(currentClient.nRows, currentClient.nCols);
    }

    refresh();
}

/**************** gameDisplay() ****************/
/*
 * Updates string based on message received by server. Called by handleMessage
 * Caller provides: 
 *   Message (such as that sent by server)
 * We do: 
 *   Print the message to the window, copy the map, move the cursor accordingly
 * We assume: 
 *   The message is not malformed (is of form DISPLAY\n[map]])
 */
void gameDisplay(const char* message, void* arg) {
    
    // copy message and print display message
    char* messageCopy = malloc(strlen(message) + 1);
    strcpy(messageCopy, message);

    // copy and print map
    char* map = messageCopy + strlen("DISPLAY\n");
    char* mapCopy = malloc(strlen(message) - strlen("DISPLAY\n") + 1);
    strcpy(mapCopy, map);

    int x, y;
    int cols = atoi(currentClient.nCols);
    int currX, currY;
    // get window size
    getbegyx(stdscr, y, x);
    y++;
    // move the cursor, set the values to the cursor values, create a temp for loop
    move(y, x);
    currX = x;
    currY = y;
    char temp = '\0';
    int j = 0;

    // loop through mapcopy
    for(int i = 0; i < strlen(mapCopy); i++) {
        // for each character in map copy
        temp = mapCopy[i];
        // if it is at the end of the column
        if (j == cols) {
            // adjust y, set x, and move the cursor
            currY++;
            currX = x;
            move(currY, currX);
            j = 0;
        }
        else {
            // add the character to the window, adjust x and the cursor
            addch(temp);
            currX++;
            move(currY, currX);
        }
        j++;
    }

    // refresh screen and free copies
    move(0,0);
    refresh();
    free(messageCopy);
    free(mapCopy);
}

/**************** setupWindow() ****************/
/* 
 * Initializes a window
 * We do:
 *   sets up a window with newterm, refreshes the screen
 */
static void setupWindow(){
  // initializes a screen
  newterm(NULL, stdout, stdin);
  cbreak(); 
  noecho();
  refresh();
}

/**************** handleSize() ****************/
/*
 * Ensures that the user window is big enough according to the rows and columns passed in
 *      called by gameGrid
 * Caller provides: 
 *   Number of rows and columns
 * We do: 
 *   prompt the user to make the window bigger until the map fits on the screen
 */
static void handleSize(char* nRows, char* nCols) {
  int currRow;
  int currCol;
  // store the window size (rows and columns)
  getmaxyx(stdscr, currRow, currCol);
  int rows = atoi(nRows) ;
  int cols = atoi(nCols) ;

  while(currRow < rows + 1 || currCol < cols + 1){

    // prompt the user to resize the screen
    printw("Your window must be at least %d high \n", rows);
    printw("Your window must be at least %d wide \n", cols);
    printw("Resize your window, and press Space to continue. \n");

    // if the character is a space, store window size again
    if (getch() == ' ') {
      getmaxyx(stdscr, currRow, currCol);
    }
    
    // refresh screen
    clear();
    refresh();
  }
}
