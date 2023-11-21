/* 
 * game structure to be used to hold the details of the game state
 *
 * See header file 'game.h' for detailed descriptions of each function
 */

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <stdbool.h>
#include "message.h"
#include "mem.h"
#include "file.h"
#include "grid.h"
#include "player.h"
#include "nuggets.h" 
#include "game.h"
#include "spectator.h"

/* CONSTANTS */
static const int MaxPlayers = 26;      // maximum number of players
static const int GoldTotal = 250;      // amount of gold in the game

typedef struct game {
    spectator_t* spectator;         // only holds one spectator at a time
    player_t** players;             // array of all the players in the game
    nuggets_t** nuggetArray;        // array of all the nugget piles in the game
    grid_t* initialGrid;            // stores the entire grid as it initially was (before players join)
    grid_t* gameState;              // stores the state of the grid (nuggets left, player locations, etc)
    int goldLeft;                   // the amount of gold left in the grid
    int numPiles;                   // number of gold piles
    int playerCount;                // keeps track of the number of players in the game
} game_t; 

// initialize a new game
game_t* game_new() {
    game_t* game = mem_malloc(sizeof(game_t));
    if (game!=NULL) {
        game->spectator = NULL;
        game->initialGrid = NULL;
        game->gameState = NULL;
        game->players = malloc(sizeof(player_t)*MaxPlayers);
        game->goldLeft = GoldTotal;
        game->numPiles = 0;
        game->playerCount = 0;
    }
    return game;
}

/********************* Setter functions **************************/
void game_setGrid(game_t* game, grid_t* grid) {
    if (game==NULL || grid==NULL) {
        return;
    }
    game->initialGrid = grid;

    char** source = grid_get_array(grid);
    char** arrayCopy = mem_malloc((grid_get_height(grid)+2) * sizeof(char*));
    for (int j = 0; j <= grid_get_height(grid); j++) {
        arrayCopy[j] = mem_malloc((strlen(source[j])+1)*sizeof(char));
       strcpy(arrayCopy[j], source[j]);
    }
    grid_t* gridCopy = grid_new(arrayCopy, grid_get_width(grid), grid_get_height(grid));
    game->gameState = gridCopy;
}
void game_setGoldLeft(game_t* game, int goldCount) {
    if (game==NULL || goldCount<0) {
        return;
    }
    game->goldLeft = goldCount;
}
void game_setPileNum(game_t* game, int pileCount) {
    if (game==NULL) {
        return;
    }
    game->numPiles = pileCount;
}
void game_setNuggetArray(game_t* game, nuggets_t** nuggetArray) {
    if (game==NULL) {
        return;
    }
    game->nuggetArray = nuggetArray;
}
void game_setSpectator(game_t* game, spectator_t* spectator) {
    if (game!=NULL && spectator!=NULL) {
        game->spectator = spectator;
    }
}       
void game_addPlayer(game_t* game, player_t* player) {
    if (game==NULL || player==NULL) {
        return;
    }
    game->players[game->playerCount] = player;
    game->playerCount = game->playerCount + 1;
}

/********************* Getter functions **************************/
spectator_t* game_getSpectator(game_t* game) {
    if (game!=NULL) {
        return game->spectator; 
    }
    return NULL;
}
player_t* game_getPlayer(game_t* game, char playerChar) {
    if (game!=NULL) {
        for (int i =0; i < game->playerCount; i++) {
            player_t* player = game->players[i];
            if (*player_getChar(player)==playerChar) {
                return player;
            }
        }
    }
    return NULL;
}
player_t* game_getPlayerFromAddress(game_t* game, addr_t* address) {
    if (game!=NULL && address!=NULL) {
        for (int i=0; i<game->playerCount; i++) {
            player_t* player = game->players[i];
            if (message_eqAddr(*player_getAddress(player),*address)) {       
                return player;
            }
        }
    }
    return NULL;
}
nuggets_t** game_getNuggetArray(game_t* game) {
    if (game!=NULL) {
        return game->nuggetArray;
    }
    return NULL;
}
int game_getPlayerCount(game_t* game) {
    if (game!=NULL) {
        return game->playerCount;
    }
    return -1;
}
int game_getGoldLeft(game_t* game) {
    if (game!=NULL) {
        return game->goldLeft;
    }
    return -1;
}
int game_getPileCount(game_t* game) {
    if (game!=NULL) {
        return game->numPiles;
    }
    return -1;
}
grid_t* game_getGameState(game_t* game) {
    if (game!=NULL) {
        return game->gameState;
    }
    return NULL;
}
grid_t* game_getInitialGrid(game_t* game) {
    if (game!=NULL) {
        return game->initialGrid;
    }
    return NULL;
}
player_t** game_getPlayers(game_t* game) {
    if (game!=NULL) {
        return game->players;
    }
    return NULL;
}

void game_delete(game_t* game) {
    for (int i = 0; i < game->playerCount; i++) {
        player_delete(game->players[i]);
    }
    mem_free(game->players);
    game->players = NULL;
    spectator_delete(game->spectator);
    for (int j = 0; j < game->numPiles; j++) { 
        nuggets_delete(game->nuggetArray[j]);
    }
    mem_free(game->nuggetArray);
    grid_delete(game->initialGrid);
    game->initialGrid = NULL;
    grid_delete(game->gameState);
    game->gameState = NULL;
    mem_free(game);
    game = NULL;
}


