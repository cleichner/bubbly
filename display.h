#ifndef DISPLAY_H
#define DISPLAY_H
#include "maze.h"
void display_maze(struct cell maze[WIDTH][HEIGHT], 
                  struct point pos, direction_t current_direction);
void init_display(void);
void finalize_display(void);
#endif // DISPLAY_H
