/*
* Robert Vogt (Lost at C) CS50, WI23
* This program is a unit test for the grid module. It tests
* all functions in the module
*/


#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <stdbool.h>
#include "grid.h"

int main()
{
  // testing grid_load with invalid filename
  grid_t* grid = grid_load("notRealFName");
  if (grid != NULL) {
    return 1;
  }
  grid = grid_load(NULL);
  if (grid != NULL) {
    return 1;
  }

  // test load and getter functions with valid args
  grid = grid_load("../maps/small.txt");
  printf("Width: %d, Height: %d\nString:\n%s\nChar ar (1,0) (should be +): %c\n", grid_get_height(grid), grid_get_width(grid), grid_toString(grid), grid_get_char_at(grid, 1, 0));
  // test change_char_at
  if (grid_change_char_at(grid, 40, 40, 'A') == true) {
    return 1;
  }
  if (grid_change_char_at(grid, 2, 2, 'A') == false) {
    return 1;
  }
  printf("New grid string:\n%s\n", grid_toString(grid));
  grid_delete(grid);

  // test grid_new and getter functions with valid args
  char* testARR[]=["test","----","____","TEST"];
  grid = grid_new(testArr, 4, 4);
  printf("Width: %d, Height: %d\nString:\n%s\nChar ar (1,0) (should be +): %c\n", grid_get_height(grid), grid_get_width(grid), grid_toString(grid), grid_get_char_at(grid, 1, 0));
  // test change_char_at
  if (grid_change_char_at(grid, 40, 40, 'A') == true) {
    return 1;
  }
  if (grid_change_char_at(grid, 2, 2, 'A') == false) {
    return 1;
  }
  printf("New grid string:\n%s\n", grid_toString(grid));
  grid_delete(grid);

  return 0;
}