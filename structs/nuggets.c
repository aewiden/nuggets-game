#include <stdio.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include "message.h"

//#include "grid.h"
//#include "player.h"

typedef struct nuggets {
    int value;
    int x;
    int y;
} nuggets_t;

nuggets_t*
nuggets_init(int value, int x, int y)
{
  if (value >= 0) {
    nuggets_t* nugget = malloc(sizeof(int)*3);
    nugget->value = value;
    nugget->x = x;
    nugget->y = y;
    return nugget;
  }
  return NULL; 
}


void
nuggets_delete(nuggets_t* nugget)
{
  if (nugget != NULL) {
    free(nugget);
  }
}

int nuggets_getX(nuggets_t* nugget)
{
    if(nugget != NULL) {
        return nugget->x;
    }
    return -1;
}

int nuggets_getY(nuggets_t* nugget)
{
    if(nugget != NULL) {
        return nugget->y;
    }
    return -1;
}

int nuggets_getValue(nuggets_t* nugget)
{
    if(nugget != NULL) {
        return nugget->value;
    }
    return -1;
}



