/*
 * game.h - header file for nuggets 'game' module 
 *
 * the game struct holds the state of the 'nuggets' game. This includes
 * keeping track of all the players, the spectator, the number of nuggets
 * left, the number of piles left, and more. 
 *
 */

#ifndef __GAME_H
#define __GAME_H

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <stdbool.h>
#include "mem.h"
#include "file.h"
#include "grid.h"
#include "player.h"
#include "nuggets.h" 
#include "spectator.h"

/****************** global types ***********************/
typedef struct game game_t;

/****************** functions **************************/

/****************** game_new ***************************/
/* Create a new game structure
 *
 * Returns a pointer to a new game
 * All of the arrays and both grids are initialized to NULL, 
 * the gold count is initialized to GoldTotal, player count and
 * number of piles of nuggets are initialized to 0.
 * 
 * Caller is responsible for later calling game_delete
 */
game_t* game_new();

/****************** setters **************************/

/****************** game_setGrid ***********************/
/* Sets the initial grid and game state grid
 * The initial grid only holds the nugget piles whereas
 * the game state grid updates as players join and collect gold
 * 
 * Caller provides valid game and grid pointers
 * function creates a copy of the passed grid before assigning
 * it to gameState so that changes to this grid does not affect the
 * initialGrid
 * 
 * grids are later freed in game_delete
 */
void game_setGrid(game_t* game, grid_t* grid);

/**************** game_setGoldLeft *********************/
/* Sets the goldLeft of the passed game to the passed goldCount
 * 
 * Caller provides:
 *      valid game pointer and int gold count
 * We guarantee:
 *      the game's goldLeft is updated to goldCount
 */
void game_setGoldLeft(game_t* game, int goldCount);

/**************** game_setPileNum *********************/
/* Sets the passed game's numPiles to the passed pileCount
 *
 * Caller provides:
 *      a valid game pointer, valid int of number of piles
 * We guarantee:
 *      the game's numPiles is assigned to the passed pileCount
 */
void game_setPileNum(game_t* game, int pileCount);

/*************** game_setNuggetArray ********************/
/* Sets the game's nuggetArray to the passed nuggetArray
 *
 * Caller provides:
 *      a valid game pointer, valid array of pointers to nugget structs
 * We guarantee:
 *      the passed game's nuggetArray will be assigned to the passed nuggetArray
 *      if neither the game nor the nuggetArray are null
 * the nuggetArray and each nugget struct is freed when game delete is called
 */
void game_setNuggetArray(game_t* game, nuggets_t** nuggetArray);

/**************** game_setSpectator *********************/
/* Sets the passed game's specator to the passed spectator struct pointer
 *
 * Caller provides:
 *      a valid game pointer, a valid spectator struct pointer
 * We guarantee:
 *      the game's spectator will be set so long as the passed arguments are not NULL
 * The spectator is later freed in game delete
 */
void game_setSpectator(game_t* game, spectator_t* spectator);

/***************** game_addPlayer ***********************/
/* Adds the passed player struct to the game's array of players
 * and updates the game's player count
 *
 * Caller provides:
 *      a valid game pointer, a valid player_t pointer
 * We guarantee:
 *      the player count and players array will be updated
 *      if the game and player are not NULL
 * each player and the player array are all freed in game_delete
 */
void game_addPlayer(game_t* game, player_t* player);

/****************** getters **************************/

/**************** game_getSpectator *********************/
/* Returns a pointer to the game's spectator 
 *  
 * Caller provides:
 *      valid game pointer
 * We return:
 *      a pointer to the game's spectator, if one exists
 *      NULL if there is not spectator
 * Spectator is freed whne game_delete is called
 */
spectator_t* game_getSpectator(game_t* game);

/***************** game_getPlayer ***********************/
/* Return a player from a passed character
 *
 * Caller provides:
 *      a valid game pointer, a character 'A-Z'
 * We return:
 *      a pointer to the player struct with the corresponding 
 *      character
 *      NULL if the player does not exist in the game's player array
 * Each player is deleted when game_delete is called
 */
player_t* game_getPlayer(game_t* game, char playerChar);

/*************** game_getPlayerFromAddress *****************/
/* Return a pointer to the player struct corresponding to the passed address
 *
 * Caller provides:
 *      a valid game pointer, a valid address pointer
 * We return:
 *      a pointer to a player struct corresponding to the address
 *      NULL if the player does not exist in the game's player array
 * Each player is deleted when game_delete is called
 */
player_t* game_getPlayerFromAddress(game_t* game, addr_t* address);

/*************** game_getNuggetArray *****************/
/* Return the game's array of nugget pointers 
 *
 * Caller provides:
 *      a valid game pointer
 * We return:
 *      the passed game's array of pointers to the nugget structs
 *      NULL if game is NULL
 * nugget array is freed when game_delete is called
 */
nuggets_t** game_getNuggetArray(game_t* game);

/**************** game_getPlayerCount *****************/
/* Return the number of players active in the game
 *
 * Caller provides:
 *      a valid game pointer
 * We return:
 *      the game's playerCount
 *      -1 if the game is NULL
 */
int game_getPlayerCount(game_t* game);

/*************** game_getGoldLeft *******************/
/* Return the amount of gold left in the game
 *
 * Caller provides:
 *      a valid game pointer
 * We return:
 *      the game's goldLeft int that keeps track of the gold 
 *      left in the game
 *      -1 if the game is NULL
 */
int game_getGoldLeft(game_t* game);

/*************** game_getPileCount *******************/
/* Return the number of piles left in the game
 *
 * Caller provides:
 *      a valid game pointer
 * We return:
 *      the game's numPiles int that keeps track of the gold 
 *      piles left in the game
 *      -1 if the game is NULL
 */
int game_getPileCount(game_t* game);

/*************** game_getGameState *******************/
/* Return the grid pointer struct that represents the game's state
 *
 * Caller provides:
 *      a valid game pointer
 * We return:
 *      a pointer to the grid that represents the game state
 *      (contains the players, updated gold piles, etc)
 *      NULL if the game is NULL
 * gameState is freed in game_delete
 */
grid_t* game_getGameState(game_t* game);

/*************** game_getInitialGrid *******************/
/* Return the grid pointer struct that represents the game's 
 * initial set up (the full grid with the original nugget piles)
 *
 * Caller provides:
 *      a valid game pointer
 * We return:
 *      a pointer to the grid that represents the game's initial setup
 *      NULL if the game is NULL
 * initial grid is freed in game_delete
 */
grid_t* game_getInitialGrid(game_t* game);

/***************** game_getPlayers *******************/
/* Return the array of pointers to players in the game
 *
 * Caller provides:
 *      a valid game pointer
 * We return:
 *      the array of players active in the game
 *      NULL if the game is NULL
 * the player array and each player is freed in game_delete
 */
player_t** game_getPlayers(game_t* game);

/*************** game_delete *******************/
/* delete the game structure
 *
 * Caller provides:
 *      a valid game pointer
 * We guarantee:
 *     each player in the player array is deleted,
 *      the entire player array is deleted, the spectator
 *      is deleted, each nugget struct is deleted, the entire
 *      nugget array is freed, each grid is deleted, and
 *      finally the game is deleted
 */
void game_delete(game_t* game);

#endif