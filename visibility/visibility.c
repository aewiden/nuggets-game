// Visibility program for nuggets 
// No Makefile yet

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>
#include "grid.h"

bool is_visible(grid_t* grid, const int pc, const int pr, const int c, const int r)
bool static is_gamespot(grid_t* grid, const int c, const int r)
bool static is_integer(const double val)
bool static is_gridpoint(const double val, const double  val2)
grid_t* return_visible(char* map, int width, int height, int x, int y);
void join_two_grid(grid_t* og, grid_t* merge, int width, int height);


bool is_visible(grid_t* grid, const int pc, const int pr, const int c, const int r){
    double change_y = r - pr;
    double change_x = c - pc;
    double slope = 0;
    double constant, c_start, c_end, r_start, r_end;
    if( change_x == 0){ // vertical line                                                                                                                                                                     
        c_start = 0;
        c_end = 0;
        if( pr > r) {
          r_start = r + 1;
          r_end = pr;
        } else {
            r_start = pr +1;
            r_end = r;
        }
    }else if( change_y == 0){ // horizontal line                                                                                                                                                             
        r_start = 0;
        r_end = 0;
        if( pc > c){
            c_start = c + 1;
            c_end = pc;
        } else {
            c_start = pc +1;
            c_end = c;
        }
    } else {
        slope = (change_y / change_x);
        constant = pr - (slope * pc);                                                                                                                                   
        c_start = 0;
        c_end = 0;
        r_start = 0;
        r_end = 0;
        if(pc > c) {
            c_start = c + 1;
            c_end = pc ;
        } else if(pc < c) {
            c_start = pc + 1;
            c_end = c  ;
        }

        if(pr > r){
            r_start = r + 1;
            r_end = pr;
        } else if(pr < r){
            r_start = pr + 1;
            r_end = r;
        }
    }                                                                                                                                                                  
    for(; c_start < c_end; c_start++){
        double newY;
        // if the line is horizontal                                                                                                                                                                          
        if(r_start == 0 && r_end == 0){
            newY = pr;
        } else {
            newY = slope * c_start + constant;
        }                                                                                                                                                                

        // check for grid point                                                                                                                                                                               
        if( is_gridpoint(c_start, newY) ) {
            if( !( is_gamespot(grid, c_start, newY))){
                return false;
            }                                                                                                                                                                           
        } else {

        // if not grid point, check the top and bottom                                                                                                                                                      
            if(!( is_gamespot(grid, c_start, floor(newY)) || is_gamespot(grid, c_start, ceil(newY)))){
                return false;
            }
        }
    }                                                                                                                                                                       
    for(; r_start < r_end; r_start++){
        double newX;
        // if the line is vertical
        if(c_start == 0 && c_end == 0) {
            newX = pc;
        } else {
            newX =( r_start - constant ) / (slope);
        }
    // check for grid point                                                                                                                                                                               
        if( is_gridpoint(newX, r_start)) {
            if(!(is_gamespot(grid, newX, r_start))){
                return false;
            }                                                                                                                                                                           
        } else {
            // if not in grid point, check the top and bottom                                                                                                                                                   
            if( !( is_gamespot(grid, floor(newX), r_start) || is_gamespot(grid, ceil(newX), r_start))){
                return false;
            }
        }
    }
    return(true);
}

bool static is_gamespot(grid_t* grid, const int c, const int r){
    if( '.' == grid_get_char_at(grid, c, r) || '*' == grid_get_char_at(grid, c, r) ){
        return true;
    }
    if( isalpha(grid_get_char_at(grid,c,r))){
        return true;
    }
    return false;
}

bool static is_integer(const double val){
    int newVal = (int)val;
    return (newVal == val);
}

bool static is_gridpoint(const double val, const double val2){
    return( (is_integer(val)) && (is_integer(val2)));
}

grid_t* return_visible(char* map, int width, int height, int x, int y){
    grid_t* grid = grid_load(map);
    for(int i = 1; i < width + 1; i++){
        for(int j = 1; j < height + 1 ; j++){
            if(x == i && j == y){
                grid_change_char_at(grid, i,j, '@');
            } else if(!is_visible(grid, x,y, i, j)) {
           grid_change_char_at(grid, i, j, ' ');
            }
        }
    }                                                                                                                                                                                

  return grid;
}

void join_two_grid(grid_t* old, grid_t* merge, int width, int height){
    for(int i = 1; i < width + 1; i++){
        for(int j = 1; j < width + 1 ; j++){
            char merge_temp = grid_get_char_at(merge, i,j);
            char old_temp = grid_get_char_at(old, i,j);
            if(old_temp == '*' && merge_temp != '*' ){
                if(merge_temp != '@'){
                    grid_change_char_at(old, i,j, '.');
                } else {
                    grid_change_char_at(old, i,j, '@');
                }
            } else if( isalpha(old_temp) && !isalpha(merge_temp)){
                if(merge_temp != '@'){
                    grid_change_char_at(old, '.', i,j, '.');
            } else {
                grid_change_char_at(old, i,j, '@');
            }
            }else if(merge_temp != ' ' ){
                grid_change_char_at(old, i,j, merge_temp);
            }
        }
    }
}
