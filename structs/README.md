# CS50 Nuggets Project
# Team name: Lost at C
## CS50 Winter 2023


### Structs
Structs is a library containing modules used throughout the implementation of the Nuggets game. The modules track the _game_, the _grid_, 
the piles of _nuggets_, whether the user is a _player_ or a _spectator_, and the _visibility_ of the grid. 

### Usage


The *structs* lib implements a _grid_ module, mostly consisting of getters and setters to keep track of the grid:
```c
typedef struct grid grid_t;
grid_t* grid_new(char** array, int width, int height);
grid_t* grid_load(char* filename);
char** grid_get_array(grid_t* grid);
int grid_get_height(grid_t* grid);
int grid_get_width(grid_t* grid);
char grid_get_char_at(grid_t* grid, int x, int y);
bool grid_change_char_at(grid_t* grid, int x, int y, char c);
char* grid_toString(grid_t* grid);
void grid_delete(grid_t* grid);
```


The *structs* lib implements a _game_ module, mostly consisting of getters and setters to keep track of the game state:
```c
typedef struct game game_t;
game_t* game_new();
void game_setGrid(game_t* game, grid_t* grid);
void game_setGoldLeft(game_t* game, int goldCount);
void game_setPileNum(game_t* game, int pileCount);
void game_setNuggetArray(game_t* game, nuggets_t** nuggetArray);
void game_setSpectator(game_t* game, spectator_t* spectator);
void game_addPlayer(game_t* game, player_t* player) ;
spectator_t* game_getSpectator(game_t* game);
player_t* game_getPlayer(game_t* game, char playerChar);
player_t* game_getPlayerFromAddress(game_t* game, addr_t* address) ;
nuggets_t** game_getNuggetArray(game_t* game);
int game_getPlayerCount(game_t* game);
int game_getGoldLeft(game_t* game);
int game_getPileCount(game_t* game);
grid_t* game_getGameState(game_t* game);
grid_t* game_getInitialGrid(game_t* game);
player_t** game_getPlayers(game_t* game);
void game_delete(game_t* game);
```

The *structs* lib implements a _nuggets_ module, which creates piles of gold for the map:
```c
typedef struct nuggets nuggets_t;
nuggets_t* nuggets_init(int value, int x, int y);
void nuggets_delete(nuggets_t* nugget);
int nuggets_getX(nuggets_t* nugget);
int nuggets_getY(nuggets_t* nugget);
int nuggets_getValue(nuggets_t* nugget);
```

The *structs* lib implements a _player_ module, which keeps track of a game player:
```c
player_t* player_new(int x, int y, char* name, addr_t* address);
int player_getX(player_t* player);
int player_getY(player_t* player);
int player_getPurse(player_t* player);
char* player_getName(player_t* player);
char* player_getChar(player_t* player);
addr_t* player_getAddress(player_t* player);
grid_t* player_getGrid(player_t* player);
void player_updatePos(player_t* player, int x, int y);
void player_updatePurse(player_t* player, int nuggets);
bool player_setChar(player_t* player, char c);
bool player_setAddress(player_t* player, addr_t address);
bool player_setGrid(player_t* player, grid_t* grid);
void player_delete(player_t* player);
```

The *structs* lib implements a _spectator_ module, which holds the address of a game spectator:
```c
typedef struct spectator spectator_t;
spectator_t* spectator_new(addr_t* address);
addr_t* spectator_getAddress(spectator_t* spectator);
void spectator_delete(spectator_t* spectator);
```

The *structs* lib implements a _visibility_ module, which tracks the visibility of the grid based on the user:
```c
bool is_visible(grid_t* grid, const int pc, const int pr, const int c, const int r);
grid_t* return_visible(grid_t* grid, int width, int height, int x, int y);
void join_two_grid(grid_t* old, grid_t* merge, int width, int height);
```



### Implementation


We implemented the *structs* lib through six modules.

The game module..

The grid module provides the data structure to represent a grid in the Nuggets game. A 'grid' contains a width, height, and an array of strings representing a game state. The grid can be loaded from a file or initialized using a given width, height, and array. Individual characters in the grid can be retrieved or changed with x and y coordinated (indexed from top left).

The nuggets module provides the data structure to represent a nugget in the Nuggets game. Each nugget keeps track of the pile's x and y location and the amount of gold it represents.

The player module provides the data structure to represent a player in the Nuggets game. Each player keeps track of its name, location, the amount of gold in its purse, its grid, and its character.

The spectator module provides the data structure to represent a spectator in the Nuggets game. Each spectator keeps track of its address,
and can be instantiated and deleted.

The visibility module contains functions to determine how much of the grid should be visible to the game user, depending on if they are a spectator or a player.



### Assumptions


No assumptions beyond those that are clear from the specifications are made in the header files.


### Files


* `Makefile` - compilation procedure
* `game.h` - the interface
* `game.c` - the implementation
* `grid.h` - the interface
* `grid.c` - the implementation of the game grid 
* `player.h` - the interface
* `player.c` - the implementation of the game player
* `spectator.h` - the interface
* `spectator.c` - the implementation of the game spectator
* `visibility.h` - the interface
* `visibility.c` - the implementation of the visibility of the grid
* `nuggets.h` - the interface
* `nuggets.c` - the implementation of the nugget piles


### Compilation

To compile, simply type `make` in command line.


### Testing


To test, run the `testing.sh` scripts in the structs directory through the command line. 
 
To test with valgrind, simply `make valgrind` in the server directory.
