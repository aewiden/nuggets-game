/* Group 13 - Lost at C
 * visibility.h - header file for the visibility program
 * 
 */
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>
#include "grid.h"

/********** is_visible *********/
/* creates a line between the player coordinates and each of the coordinates on the map
 * increments from the lower coordinate to the higher coordinate and checks if the line is obstructed by any obstacles
 * if the line is not obstructed, the coordinate is visible to the player
 */
bool is_visible(grid_t* grid, const int pc, const int pr, const int c, const int r);

/********* return_visible ********/
/* runs the is_visible method across the entire map then returns a grid representative of the player's visibility
 */
grid_t* return_visible(char* map, int width, int height, int x, int y);

/********* join_two_grid **********/
/* takes in the old grid and the current grid and merges them together
 */
void join_two_grid(grid_t* old, grid_t* merge, int width, int height);