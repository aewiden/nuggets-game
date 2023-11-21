/*
* Header file for the Nuggets 'player' module
* 
* A 'player' is a module providing the data structure to represent a player in the Nuggets game. 
* Each player keeps track of its name, location, the amount of gold in its purse, its grid, and its character.
*
* CS50, Winter 2023 
*/

#ifndef __PLAYER_H
#define __PLAYER_H

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <stdbool.h>
#include "mem.h"
#include "message.h"

/**************** global types ****************/
typedef struct player {
    int x;
    int y;
    int purse;
    char* name;
    addr_t* address;
    char playerChar;
    grid_t* visibleGrid;
} player_t;

player_t* player_new(int x, int y, char* name, addr_t* address);

/* getter methods */
int player_getX(player_t* player);
int player_getY(player_t* player);
int player_getPurse(player_t* player);
char* player_getName(player_t* player);
char* player_getChar(player_t* player);
addr_t* player_getAddress(player_t* player);
grid_t* player_getGrid(player_t* player);

/* setter methods */
void player_updatePos(player_t* player, int x, int y);
void player_updatePurse(player_t* player, int nuggets);
bool player_setChar(player_t* player, char c);
bool player_setAddress(player_t* player, addr_t address);
bool player_setGrid(player_t* player, grid_t* grid);
void player_delete(player_t* player);

void player_delete(player_t* player);


/**************** player_new() ****************/
/* A function to create a new player struct. */
/* 
* Caller provides: 
*   the name of the player, its x and y location, and the address
* We do: 
*   allocate memory for a new player 
*   choose a random room spot for the player 
*   initialize player purse
* We return: 
*   the player, if everything is successful, 
*   NULL, on error
*/
player_t* player_new(int x, int y, char* name, addr_t* address);

/**************** player_getX() ****************/
/* A function to return the x coordinate of a player */
/* 
* Caller provides: 
*   a player object
* We return: 
*   if the player is not null, the x coordinate of the player
*   if the player is null, return -1  
*/
int player_getX(player_t* player);

/**************** player_getY() ****************/
/* A function to return the y coordinate of a player */
/* 
* Caller provides: 
*   a player object
* We return: 
*   if the player is not null, the y coordinate of the player
*   if the player is null, return -1  
*/
int player_getY(player_t* player);

/**************** player_getPurse() ****************/
/* A function to return the amount of gold a player has in their purse */
/* 
* Caller provides: 
*   a player object
* We return: 
*   if the player is not null, the amount of gold in this player's purse
*   if the player is null, return -1  
*/
int player_getPurse(player_t* player);

/**************** player_getName() ****************/
/* A function to return the real name of a player*/
/* 
* Caller provides: 
*   a player object
* We return: 
*   the player's real name, if the player is not null 
*    null otherwise 
*/
char* player_getName(player_t* player);

/**************** player_getChar() ****************/
/* A function to return the char of a player*/
/* 
* Caller provides: 
*   a player object
* We return: 
*   the player's char, if the player is not null 
*    null otherwise 
*/
char* player_getChar(player_t* player);

/**************** player_getAddress() ****************/
/* A function to return the address of a player*/
/* 
* Caller provides: 
*   a player object
* We return: 
*   the player's address, if the player is not null 
*    null otherwise 
*/
addr_t* player_getAddress(player_t* player);

/**************** player_getGrid() ****************/
/* A function to return the grid object of a player*/
/* 
* Caller provides: 
*   a player object
* We return: 
*   the player's grid object, if the player is not null 
*    null otherwise 
*/
grid_t* player_getGrid(player_t* player);



void player_updatePos(player_t* player, int x, int y);

/**************** player_updatePurse() ****************/
/* A function to add to the gold count of a player. */
/* 
* Caller provides: 
*   a player object, an integer representing the amount of gold to add
* We do: 
*   if the player is not null, add numGold amount of gold to the player's numGold attribute 
* We return: 
*   nothing
*/
void player_updatePurse(player_t* player, int nuggets);

/**************** player_setChar() ****************/
/* A function to change the character of a player */
/* 
* Caller provides: 
*   a player object, a char to switch to
* We do: 
*   change the player's letter to this char
*  
*/
bool player_setChar(player_t* player, char c);

/**************** player_setAddress() ****************/
/* A function to change the address of a player */
/* 
* Caller provides: 
*   a player object
*   an addr_t object
* We do: 
*   change the player's address to this address, as long as the given player and address aren't NULL
*/
bool player_setAddress(player_t* player, addr_t address);

/**************** player_setGrid() ****************/
/* A function to change the grid of a player */
/* 
* Caller provides: 
*   a player object 
*   a grid_t object
* We do: 
*   change the player's grid to this grid, as long as the given player and grid aren't NULL
*/
bool player_setGrid(player_t* player, grid_t* grid);

/**************** player_delete() ****************/
/* A function to delete a player struct and free all of its memory */
/*
* We do: 
*   free player name 
*   free player 
* We return: 
*   nothing
*/
void player_delete(player_t* player);

#endif