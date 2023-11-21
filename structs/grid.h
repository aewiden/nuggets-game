/* 
 * grid.h - header file for 'grid' module
 * 
 * A 'grid' contains a width, height, and an array of strings representing a game
 * state. The grid can be loaded from a file or initialized using a given width,
 * height, and array. Individual characters in the grid can be retrieved or changed
 * with x and y coordinated (indexed from top left).
 *
 * Robert Vogt (Lost at C) CS50 WI23
 */

#ifndef __GRID_H
#define __GRID_H

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include "mem.h"
#include "file.h"
#include <string.h>
#include <stdbool.h>

/**************** global types ****************/
typedef struct grid{
  int width;
  int height;
  char** array;
}grid_t;

/**************** functions ****************/

/**************** grid_new ****************/
/* Create a new grid using given width, height, and array.
 *
 * We return:
 *   pointer to a new grid, or NULL if error
 * Caller is responsible for:
 *   providing correct width and height
 *   later calling grid_delete
 */
grid_t* grid_new(char** array, int width, int height);

/**************** grid_load ****************/
/* Create a new grid from the given filename.
 *
 * We return:
 *   pointer to a new grid, or NULL if error
 * Caller is responsible for:
 *   later calling grid_delete
 */
grid_t* grid_load(char* filename);

/**************** grid_get_array ****************/
/* Return the array of strings contained within the given grid.
 *
 * Caller provides:
 *   valid grid pointer
 * We return:
 *   return grid's array, or NULL if grid is NULL or grid has no array
 */
char** grid_get_array(grid_t* grid);

/**************** grid_get_height ****************/
/* Return the value for height contained within the given grid.
 *
 * Caller provides:
 *   valid grid pointer
 * We return:
 *   return grid's height, or -1 if grid is NULL
 */
int grid_get_height(grid_t* grid);

/**************** grid_get_width ****************/
/* Return the value for width contained within the given grid.
 *
 * Caller provides:
 *   valid grid pointer
 * We return:
 *   return grid's width, or -1 if grid is NULL
 */
int grid_get_width(grid_t* grid);

/**************** grid_get_char_at ****************/
/* Return the character at the coordinates (x,y) in the given grid.
 *
 * Caller provides:
 *   valid grid pointer
 *   x and y coordinates
 * We return:
 *   return character at the specified coordinates, or null char if coordinates are
 *   invalid or grid is NULL
 */
char grid_get_char_at(grid_t* grid, int x, int y);

/**************** grid_change_char_at ****************/
/* Change the character at the coordinates (x,y) in the given grid.
 *
 * Caller provides:
 *   valid grid pointer
 *   x and y coordinates
 *   character to insert at specified coordinates
 * We return:
 *   return true if character at given location was sucessfully replaced,
 *   false otherwise
 */
bool grid_change_char_at(grid_t* grid, int x, int y, char c);

/**************** grid_toString ****************/
/* Return a string containing the state of the given grid
 *
 * Caller provides:
 *   valid grid pointer
 * We return:
 *   the grid's array in a single string, or NULL if the provided grid or the grid's
 *   array are NULL
 * Caller is responsible for:
 *   later calling free on the returned string
 */
char* grid_toString(grid_t* grid);

/**************** grid_delete ****************/
/* Free all memory associated with the given grid
 *
 * Caller provides:
 *   valid grid pointer
 */
void grid_delete(grid_t* grid);

#endif // __GRID_H