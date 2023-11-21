/*
 * server.c - implementation of the server module
 * 
 * CS50 W23 Nuggets Project
 * Team name: Lost at C
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

/* CONSTANTS */
static const int MaxNameLength = 50;   // max number of chars in playerName
static const int MaxPlayers = 26;      // maximum number of players
static const int GoldTotal = 250;      // amount of gold in the game
static const int GoldMinNumPiles = 10; // minimum number of gold piles
static const int GoldMaxNumPiles = 30; // maximum number of gold piles

static void parseArgs(const int argc, char* argv[]);
bool initializeGame(grid_t* grid) ;
bool handleMessage(void* arg, const addr_t from, const char* message);
bool handleKeypress(void* arg);
bool movePlayer(player_t* player, const int xchange, const int ychange);
void checkNuggets(player_t* player, const int xchange, const int ychange);
void endGame();
void updateDisplay(player_t* player);

/* GLOBAL VARIABLE */
static game_t* game;                    // global game variable

/**************** local types ****************/
/* holds an address and keystroke to pass this
 * information to handleKeypress()
 */
typedef struct arg {
    addr_t address;
    char key;
} arg_t;

// caller is responsible for freeing arg later
arg_t* arg_new(addr_t address, char key) {
    arg_t* arg = malloc(sizeof(arg_t));
    if (arg!=NULL) {
        arg->address = address;
        arg->key = key;
        return arg;
    }
    return NULL;
}

/************************* main ************************/
/* validates arguments, intializes network, begins loop for 
 * client messages
 *
 * returns 0 if successful run of program
 * returns nonzero if errors occur
 */
int main(int argc, char* argv[]) {
    parseArgs(argc, argv);                      // validate arguments
    log_init(stderr);
    bool ok;
    int port = message_init(stderr);                  // random port number
    fprintf(stdout, "Ready to play, waiting at port %d\n", port);
    if (port==0) {
        log_e("Error creating port");
        return 1;
    }
    else { 
        ok = message_loop(NULL, 0, NULL, NULL, handleMessage);     // NULL timeout and NULL stdin functions (bc server only receives messages from client)
        if (!ok) {                                                  // error in message_loop
            endGame();
            return 5;
        }
        message_done();
        log_done();
        return 0;
    }
    return 1;
}

/********************* parse args *************************/
/*
* inputs: command-line arguments received by main
* Checks that the map filename and seed (if one is provided) are valid, initializes 
* a grid from the filename if it is valid, generates a seed if one is not provided,
* and calls initializeGame.
* Exits non-zero if the command-line arguments are invalid or an error is
* encountered
*/
static void parseArgs(const int argc, char* argv[]) {
    if (argc>=2) {      // provided optional [seed] argument
        if (argc==3) {
            char* seedString = argv[2];
            for (int i=0; i<strlen(seedString); i++) {                      // validate seed argument
                if (!isdigit(argv[2][i])){
                    log_e("Seed argument is not a valid number");
                    exit(2);
                }   
            }
            int seed = atoi(seedString);
            srand(seed);
        } 
        else if (argc==2) {
            srand(getpid());                                  
        }
        else {
            log_e("Too many command line arguments\n");
            exit(1);
        }
    }
    else {                                                              // not enough arguments
        log_e("Usage: ./server map.txt [seed]\n");
        exit(1);
    }

    char* filename = argv[1];
    grid_t* grid = grid_load(filename);         // load the grid
    if (grid!=NULL) {
        if (!initializeGame(grid)) {
            log_e("Error initializing game");;
            exit(1);
        }
    }
    else {
        log_e("Error: could not load grid from file\n");
        exit(2);
    }
}

/**************************** initializeGame ****************************/
/* initializes a new game struct
 * adds nugget piles to the grid and sets pile count, grids, and nugget array in game struct
 *
 * Caller provides:
 *      a valid grid pointer
 * We return:
 *      false if the game cannot be initialized 
 *      true 
 */
bool initializeGame(grid_t* grid) {
    game = game_new();                    
    if (game==NULL) {
        return false;
    }
    nuggets_t** nuggetArray = mem_malloc(sizeof(nuggets_t*) * (GoldMaxNumPiles+1));
    for (int j = 0; j <= GoldMaxNumPiles; j++) {     // initialize array to all NULLs
        nuggetArray[j] = NULL;
    }
    int i = 0;
    int goldCount = GoldTotal;
    int height = grid_get_height(grid);
    int width = grid_get_width(grid);
    int maxNuggets = GoldTotal/GoldMinNumPiles;                         // should return 25
    int minNuggets = (int) ceil(GoldTotal/GoldMaxNumPiles);             // should return 9
    int nuggetCount;
    int x = 0;
    int y = 0;
    // Initialize the game by dropping at least GoldMinNumPiles and at most GoldMaxNumPiles gold piles on random room spots; each pile shall have a random number of nuggets.
    while (goldCount>=25) {       // loop until there are less than 25 nuggets left
        nuggetCount = (rand() % (maxNuggets - minNuggets + 1)) + minNuggets;             // random number between min and max
        while (grid_get_char_at(grid, x, y)!='.') {                 // loop until it finds a valid spot in the grid
            x = rand() % width;                                     // random number between 0 and width
            y = rand() % height;                                    // random number between 0 and height
        }
        grid_change_char_at(grid, x, y, '*');                       // change character from . to * for gold pile
        nuggets_t* nuggets = nuggets_init(nuggetCount, x, y);
        if (nuggets!=NULL) {
            nuggetArray[i] = nuggets;                               // add nugget pile to nugget array    
            goldCount = goldCount - nuggetCount;
            i++;
        }
        else {
            log_e("Error: could not initialize new nugget struct\n");
            return 6;
        }
    }
    // put all remaining nuggets into one final pile.
    if (goldCount!=0) {
        while (grid_get_char_at(grid, x, y)!='.') {                     // loop until it finds a valid spot in the grid
        x = rand() % width;                                         // random number between 0 and width
        y = rand() % height;                                        // random number between 0 and height
    }
        nuggets_t* nuggets = nuggets_init(goldCount, x, y);         // assign all remaining nuggets to a pile
        grid_change_char_at(grid, x, y, '*');                       // change character from . to * for gold pile
        nuggetArray[i] = nuggets;
        i++;
    } 

    game_setPileNum(game, i);                   
    game_setNuggetArray(game, nuggetArray);                     
    game_setGrid(game, grid);                               // set initial grid with nuggets                           
    
    if (game_getInitialGrid(game)!=NULL && game_getPileCount(game)>0 && game_getNuggetArray(game)!=NULL) {
        return true;
    }  
    return false;    
}

/********************* handleMessage *************************/
/*
* inputs: address from which the message was received and a string containing the
* message's contents
* Checks if a message is a KEY, PLAY, or SPECTATE message and either calls 
* handleKeypress, creates a new player if the game is not at capacity, or creates 
* a new spectator, respectively.
* outputs: returns false in all cases
*/
bool handleMessage(void* arg, const addr_t from, const char* message) {

    if (message==NULL) {                   // check arguments -- check arg and from?
        return false;
    }
    if (strncmp(message, "KEY ", strlen("KEY "))==0) {                  // compares the first 4 chars of message to 'KEY '
        char* content = (char*) message + strlen("KEY ");                 // starts from the index after 'KEY '
        if (strlen(content)!=1){
            printf("did not get char\n");
            return false;
        }
        arg_t* arg = arg_new(from, *content);                            // initialize a new arg_t with address and key press
        bool status = handleKeypress(arg);
        free(arg);
        return status;
    }
    else if (strncmp(message, "PLAY ", strlen("PLAY "))==0) {
        char* name = malloc(sizeof(char)*(strlen(message)-strlen("PLAY ")+1));
        name[0] = '\0';
        strcpy(name, (char*)message + strlen("PLAY "));
        if (strlen(name)<1 || name==NULL) {                                        // check if name was provided, if not, throw an error
            log_e("error: you must provide player name");
            return false;
        }
        else if (game_getPlayerCount(game)>=MaxPlayers) {                             // check if game already has max players
            message_send(from, "QUIT game is at maximum player capacity");
        }
        else {
            int x = 0;
            int y = 0;
            grid_t* gameState = game_getGameState(game);

            while (grid_get_char_at(gameState, x, y)!='.') {                 // loop until it finds a valid spot in the grid
                x = rand() % grid_get_width(gameState);                                     // random number between 0 and width
                y = rand() % grid_get_height(gameState);                                    // random number between 0 and height
            }
            player_t* newPlayer;

            if (strlen(name)<=MaxNameLength) {
                addr_t* tmpAddr = mem_malloc(sizeof(addr_t));
                memcpy(tmpAddr,(addr_t*)&from,sizeof(addr_t));
                newPlayer = player_new(x, y, name, tmpAddr);
                player_setChar(newPlayer, 'A' + game_getPlayerCount(game));
            }
            else {
                char* truncateName = malloc(sizeof(char)* (MaxNameLength+2)); 
                addr_t* tmpAddr = mem_malloc(sizeof(addr_t));
                memcpy(tmpAddr,(addr_t*)&from,sizeof(addr_t));
                strncpy(truncateName, name, MaxNameLength+1);
                truncateName[MaxNameLength] = '\0';
                newPlayer = player_new(x, y, truncateName, tmpAddr);
                player_setChar(newPlayer, 'A' + game_getPlayerCount(game));
                mem_free(name);
            }

            game_addPlayer(game, newPlayer);                                // increments player count, adds player to array
            
            char message[5] = "OK ";
            char* symbol = player_getChar(newPlayer);
            char tmp[2];
            tmp[0] = *symbol;
            tmp[1] = '\0';
            strcat(message, tmp);
            message[4] = '\0';
            message_send(from, message);

            char gridMessage[12];
            sprintf(gridMessage, "GRID %d %d", grid_get_height(game_getGameState(game)), grid_get_width(game_getGameState(game)));
            message_send(from, gridMessage);     // nrows, ncols
            
            char goldMessage[16];
            sprintf(goldMessage, "GOLD %d %d %d", 0, player_getPurse(newPlayer), game_getGoldLeft(game));
            message_send(from, goldMessage);     
            
            // update player and spectator display
            player_t** players = game_getPlayers(game);
            for (int i = game_getPlayerCount(game)-1; i >=0; i--) {
                player_t* currentPlayer = players[i];
                updateDisplay(currentPlayer);
            }
        }   
    }
    else if (strncmp(message, "SPECTATE", strlen("SPECTATE"))==0) {
        spectator_t* oldSpectator = game_getSpectator(game);
        if (oldSpectator!=NULL) {                                       // if there already is a spectator, tell the prior spectator to quit
            addr_t spectatorAddress = *spectator_getAddress(oldSpectator);                 // need to create spectator data structure
            message_send(spectatorAddress, "QUIT sorry a new spectator has joined so you must quit\n");
            spectator_delete(oldSpectator);
        }
        // initialize a new spectator, regardless of whether one existed or not
        addr_t* tmpAddr = mem_malloc(sizeof(addr_t));
        memcpy(tmpAddr,(addr_t*)&from,sizeof(addr_t));
        spectator_t* newSpectator = spectator_new(tmpAddr); // need to write specator data structure and then EDIT THIS
        game_setSpectator(game, newSpectator);
        
        char gridMessage[12];
        sprintf(gridMessage, "GRID %d %d", grid_get_height(game_getGameState(game)), grid_get_width(game_getGameState(game)));
        message_send(from, gridMessage);     // nrows, ncols
        
        char goldMessage[16];
        sprintf(goldMessage, "GOLD %d %d %d", 0, 0, game_getGoldLeft(game));
        message_send(from, goldMessage);

        char* displayString = grid_toString(game_getGameState(game));
        char display[strlen(displayString)+1];
        strcpy(display, displayString);
        char displayMessage[strlen(displayString) + 10];
        strcpy(displayMessage, "DISPLAY\n");
        strcat(displayMessage, display);         
        message_send(from, displayMessage);
        free(displayString);
    }
    return false;
}

/********************* handleKeypress *************************/
/*
* inputs: arg, which contains the key that was pressed and the address of the 
* player that sent the keypress
* If the key pressed is q or Q, send a QUIT message to the player; if the keypress
* indicates movement, call movePlayer and call updateDisplay if the move is valid.
* After the keypress has been handled, call endGame if all gold has been collected.
*/
bool handleKeypress(void* arg)
{   
    arg_t* tmp = arg;
    char c = tmp->key;
    const addr_t to = tmp->address;
    if (game_getSpectator(game)!=NULL) {
        spectator_t* spectator = game_getSpectator(game);
        const addr_t specAddress = *spectator_getAddress(spectator);
        if (message_eqAddr(specAddress, to)) {
            if (c=='Q') {
                message_send(to, "QUIT ");
            }
        }
    }

    player_t* player = game_getPlayerFromAddress(game, &tmp->address);
    if (player==NULL) {
        return false;
    }
    if (c == 'Q') {
        player_setChar(player, '.');
        updateDisplay(player);
        message_send(to, "QUIT ");

    } else if (c == 'h') {
        // move left
        if (movePlayer(player, -1, 0)) {
            updateDisplay(player);
        }
    } else if (c == 'l') {
        // movePlayerPlayer right
        if (movePlayer(player, 1, 0)) {
            updateDisplay(player);
        }
    } else if (c == 'k') {
        // movePlayer up
        if (movePlayer(player, 0, -1)) {
            updateDisplay(player);
        }
    } else if (c == 'j') {
        // movePlayer down
        if (movePlayer(player, 0, 1)) {
            updateDisplay(player);
        }
    } else if (c == 'y') {
        // movePlayer up and left
        if (movePlayer(player, -1, -1)) {
            // send display message
            updateDisplay(player);
        }
    } else if (c == 'u') {
        // movePlayer up and right
        if (movePlayer(player, 1, -1)) {
            // send display message
            updateDisplay(player);
        }
    } else if (c == 'b') {
        // movePlayer down and left
        if (movePlayer(player, -1, 1)) {
            // send display message
            updateDisplay(player);
        }
    } else if (c == 'n') {
        // movePlayer down and right
        if (movePlayer(player, 1, 1)) {
            // send display message
            updateDisplay(player);
        }
    } else if (c == 'H') {
        // movePlayer left
        bool goodMove = true;
        while (goodMove == true) {
            // continue moving left
            goodMove = movePlayer(player, -1, 0);
            updateDisplay(player);
        }
    } else if (c == 'L') {
        // movePlayer right
        bool goodMove = true;

        while (goodMove == true) {
            // continue moving right
            goodMove = movePlayer(player, 1, 0);
            updateDisplay(player);
        }
    } else if (c == 'K') {
        // movePlayer up
        bool goodMove = true;

        while (goodMove == true) {
            // continue moving left
            goodMove = movePlayer(player, 0, -1);
            updateDisplay(player);
        }
    } else if (c == 'J') {
        // movePlayer down
        bool goodMove = true;

        while (goodMove == true) {
            // continue moving down
            goodMove = movePlayer(player, 0, 1);
            updateDisplay(player);
        }
    } else if (c == 'Y') {
        // movePlayer up and left
        bool goodMove = true;

        while (goodMove == true) {
            // continue moving up and left
            goodMove = movePlayer(player, -1, -1);
            updateDisplay(player);
        }
    } else if (c == 'U') {
        // movePlayer up and right
        bool goodMove = true;

        while (goodMove == true) {
            // continue moving up and right
            goodMove = movePlayer(player, 1, -1);
            updateDisplay(player);
        }
    } else if (c == 'B') {
        // movePlayer down and left
        bool goodMove = true;

        while (goodMove == true) {
            // continue moving down and left
            goodMove = movePlayer(player, -1, 1);
            updateDisplay(player);
        }
    } else if (c == 'U') {
        // movePlayer down and right
        bool goodMove = true;

        while (goodMove == true) {
            // continue moving down and right
            goodMove = movePlayer(player, 1, 1);
            updateDisplay(player);
        }
    }

    if (game_getGoldLeft(game) == 0) {
        free(tmp);
        endGame();
        return true;
    }
    return false;
}

/********************* movePlayer *************************/
/*
* inputs: a player and the amount by which their x and y coordinates will be
* changed
* Call check_nuggets and check that the move is valid. If the move if valid, 
* update the player's x and y location in accordance with the input and replace 
* the character at their previous location.
* Return true if the move is valid and false otherwise.
*/
bool movePlayer(player_t* player, const int xchange, const int ychange)
{
    checkNuggets(player, xchange, ychange);
    char next = grid_get_char_at(game_getGameState(game), player_getX(player) + xchange, player_getY(player) + ychange);
    if (next != '.' && next != '#' && next != '*' && !isalpha(next)) {                            // next character is a border character
        return false;
    }
    // switch the player's positions
    if (isalpha(next)) {
        player_t* nextPlayer = game_getPlayer(game, next);
        player_updatePos(nextPlayer, player_getX(player), player_getY(player));
        grid_change_char_at(game_getGameState(game), player_getX(nextPlayer), player_getY(nextPlayer), *player_getChar(nextPlayer));
    }
    else {
        char old = grid_get_char_at(game_getInitialGrid(game), player_getX(player), player_getY(player));       // change character back to prior character
        if (old == '.' || old == '*' ) {
            grid_change_char_at(game_getGameState(game), player_getX(player), player_getY(player), '.');
        }
        else if (old == '#') {
            grid_change_char_at(game_getGameState(game), player_getX(player), player_getY(player), '#');
        }
    } 
    player_updatePos(player, player_getX(player) + xchange, player_getY(player) + ychange);
    return true;
}

/********************* check_nuggets *************************/
/*
* inputs: a player and the amount by which their x and y coordinates will be
* changed
* If the character currently occupying the coordinates that the player will be moved
* into is a nugget, find that nugget in the nugget array, send a GOLD message to 
* all players and the spectator, and delete the nugget.
*/
void checkNuggets(player_t* player, const int xchange, const int ychange)
{
    char next = grid_get_char_at(game_getGameState(game), player_getX(player) + xchange, player_getY(player) + ychange);
    if (next == '*') {
        // moving to space with nugget
        nuggets_t** nuggetArray = game_getNuggetArray(game);
        for (int i = 0; i < GoldMaxNumPiles; i++) {
            nuggets_t* nugget = nuggetArray[i];
            if (nugget == NULL) {
                continue;
            }
            if (nuggets_getX(nugget) == player_getX(player) + xchange && nuggets_getY(nugget) == player_getY(player) + ychange) {
                // found the nugget the player is moving to
                player_updatePurse(player, nuggets_getValue(nugget));
                int goldCount = game_getGoldLeft(game);
                game_setGoldLeft(game, goldCount - nuggets_getValue(nugget));
                
                const addr_t to = *player_getAddress(player);
                char goldMessage[16];
                sprintf(goldMessage, "GOLD %d %d %d", nuggets_getValue(nugget), player_getPurse(player), game_getGoldLeft(game));
                message_send(to, goldMessage);

                player_t** players = game_getPlayers(game);
                for (int i = 0; i< game_getPlayerCount(game); i++) {
                    player_t* player = players[i];
                    const addr_t dest = *player_getAddress(player);
                    if (!message_eqAddr(to, dest)) {
                        char goldUpdate[16];
                        sprintf(goldUpdate, "GOLD %d %d %d", 0, player_getPurse(player), game_getGoldLeft(game));
                        message_send(dest, goldUpdate);
                    }
                }

                if (game_getSpectator(game)!=NULL) {
                    spectator_t* spectator = game_getSpectator(game);
                    const addr_t dest = *spectator_getAddress(spectator);
                    char goldUpdate[16];
                    sprintf(goldUpdate, "GOLD %d %d %d", 0, 0, game_getGoldLeft(game));
                    message_send(dest, goldUpdate);
                }
                
                nuggets_delete(nugget);
                nuggetArray[i] = NULL;
                break;
            }
        }
    }
}

/* called after the player(s) has collected all the gold or if there is an error in message_loop
 * Creates a game summary with each player's score, character, and name
 *
 * loops through each player and creates the game summary string
 * loops through each player again to send the game symmary message
 * if there is a spectator, sends the game summary to them
 */
void endGame() {
    char* message = "QUIT GAME OVER:\n\r";
    char gameSummary[strlen(message) + (MaxNameLength+6)*(game_getPlayerCount(game)+1)];
    gameSummary[0]='\0';

    strcat(gameSummary,message);
    player_t** players = game_getPlayers(game);
    for (int i = 0; i<game_getPlayerCount(game); i++) {
        player_t* player = players[i];
        char* name = player_getName(player);
        char* c = player_getChar(player);
        c[1] = '\0';
        int score = player_getPurse(player);
        char* temp = malloc((strlen(name)+12)*sizeof(char));
        temp[0] = '\0';
        sprintf(temp, "%s %7d %s\n", c, score, name);
        strcat(gameSummary, temp);
        free(temp);
    }
    for (int j = 0; j < game_getPlayerCount(game); j++) {          // send game summary to each player
        player_t* player = players[j];
        const addr_t to = *player_getAddress(player);
        message_send(to, gameSummary);
    }
    if (game_getSpectator(game)!=NULL) {              // send game summary to spectator if there is one
        const addr_t spectAddress = *spectator_getAddress(game_getSpectator(game));
        message_send(spectAddress, gameSummary);
    }

   game_delete(game);                // free all players, spectator, nuggets, grids, etc.
   exit(0);                          // end game 
}

/* updates the game state grid and sends updates to every client
 *
 * Caller provides a valid pointer to a player
 * Everytime a player moves, this function is called to update the
 * game state grid and then broadcast the changes to all the clients
 * 
 * For each player, it also updates the visible grid in case the 
 * moving player has moved into their visible grid
 */
void updateDisplay(player_t* p) {
    //update spectator and game state
    spectator_t* spectator;
    grid_change_char_at(game_getGameState(game), player_getX(p), player_getY(p), *player_getChar(p));

    if ((spectator = game_getSpectator(game))!=NULL) {
        //grid_change_char_at(game_getGameState(game), player_getX(p), player_getY(p), *player_getChar(p));
        char* specString = grid_toString(game_getGameState(game));
        char message[strlen(specString)+1];
        strcpy(message, specString);
        char specMessage[strlen(specString) + 10];
        strcpy(specMessage, "DISPLAY\n");
        strcat(specMessage, message);         
        message_send(*spectator_getAddress(spectator), specMessage);
        free(specString);
    }

    grid_t* gameState = game_getGameState(game);            // send visible game state to each player
    player_t** players = game_getPlayers(game);
    char** source = grid_get_array(gameState);
    
   // update every player's display
    for (int i = 0; i < game_getPlayerCount(game); i++) {
        player_t* player = players[i];
        
        char** arrayCopy = mem_malloc((grid_get_height(gameState)+3) * (grid_get_width(gameState)+1) * sizeof(char));
        for (int n = 0; n < grid_get_height(gameState)+3; n++) {
            arrayCopy[n] = NULL;
        }
        const addr_t dest = *player_getAddress(player);
        for (int j = 0; j <= grid_get_height(gameState); j++) {                 // <=
            arrayCopy[j] = mem_malloc((strlen(source[j])+1)*sizeof(char));
            strcpy(arrayCopy[j], source[j]);
        }
        
        grid_t* visibleGrid = grid_new(arrayCopy, grid_get_width(gameState), grid_get_height(gameState));     
        return_visible(visibleGrid, grid_get_width(game_getGameState(game)), grid_get_height(game_getGameState(game)),player_getX(player), player_getY(player));
        
        // first time being initialized
        if (player_getGrid(player)==NULL) {
            player_setGrid(player, visibleGrid);
        }
        // need to join visibleGrid with previous visibleGrid. Joins both grids into player's grid
        else {      
            join_two_grid(player_getGrid(player), visibleGrid, grid_get_width(visibleGrid), grid_get_height(visibleGrid));
            grid_delete(visibleGrid);
        }
        char* displayString = grid_toString(player_getGrid(player));
        char display[strlen(displayString)+1];
        strcpy(display, displayString);
        char displayMessage[strlen(displayString) + 10];
        strcpy(displayMessage, "DISPLAY\n");
        strcat(displayMessage, display); 
        message_send(dest, displayMessage);
        grid_change_char_at(game_getGameState(game), player_getX(player), player_getY(player), *player_getChar(player));
        free(displayString);
    }
}
