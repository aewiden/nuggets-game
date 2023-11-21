/*
 * unit test file for 'game' struct
 *
 *
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


int main() {
    game_t* game = game_new();
    if (game==NULL) {
        fprintf(stderr, "game could not be initiaized\n");
        return 1;
    }
    grid_t* grid = grid_load("../maps/small.txt");
    game_setGrid(game, grid);
    if (game_getGameState(game)==NULL || game_getInitialGrid(game)==NULL) {
        printf("error setting game state and initial grids\n");
        return 1;
    }

    game_setGoldLeft(game, 250);
    int goldLeft = game_getGoldLeft(game);
    printf("gold left (should be 250) = %d\n", goldLeft);

    game_setPileNum(game, 25);
    int piles = game_getPileCount(game);
    printf("pile count (should be 25) = %d\n", piles);

    // test setting the nugget array
    nuggets_t** nuggetArray = malloc(sizeof(nuggets_t)*30);
    for (int i = 0; i < 25; i++) {
        nuggets_t* nugget = nuggets_init(15, i, i);
        nuggetArray[i] = nugget;
    }
    game_setNuggetArray(game, nuggetArray);
    if (game_getNuggetArray(game) == NULL) {
        printf("could not load nuggetArray\n");
        return 1;
    }

    // test adding a player
    char* name = mem_malloc(strlen("bob")+1);
    strcpy(name, "bob");
    player_t* player = player_new(1, 1, name, NULL);

    bool ok = player_setChar(player, 'A');
    if (!ok) {
        printf("not ok\n");
        return 1;
    }
    game_addPlayer(game, player);
    int count = game_getPlayerCount(game);
    printf("player count (should be 1)= %d\n", count);  

    if (game_getPlayer(game, 'A')==NULL) {
        printf("could not find player\n");
        return 1;
    }
    if (game_getPlayers(game)==NULL) {
        printf("error getting players array\n");
        return 1;
    }

    // test game delete function
    game_delete(game);
    printf("delete worked\n");
    return 0;
}