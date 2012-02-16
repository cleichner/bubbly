#include <stdio.h>

#include "maze.h"
#include "display.h"

void init_display(void) {
    puts("initialized");
}

void finalize_display(void) {
    puts("finalized");
}

void display_maze(struct cell maze[WIDTH][HEIGHT], 
                  struct point pos, direction_t current_direction, bool fast) {
    char dir_string[4][6] = {"North", "South", "East ", "West "};
    printf("(%d, %d) facing %s connected to ", pos.x, pos.y,
           dir_string[current_direction]);
    if (maze[pos.x][pos.y].path[NORTH])
        printf("| (%d, %d) to the North ", maze[pos.x][pos.y].path[NORTH]->x, 
               maze[pos.x][pos.y].path[NORTH]->y);
    if (maze[pos.x][pos.y].path[SOUTH])
        printf("| (%d, %d) to the South ", maze[pos.x][pos.y].path[SOUTH]->x, 
               maze[pos.x][pos.y].path[SOUTH]->y);
    if (maze[pos.x][pos.y].path[EAST])
        printf("| (%d, %d) to the East  ", maze[pos.x][pos.y].path[EAST]->x, 
               maze[pos.x][pos.y].path[EAST]->y);
    if (maze[pos.x][pos.y].path[WEST])
        printf("| (%d, %d) to the West  ", maze[pos.x][pos.y].path[WEST]->x, 
               maze[pos.x][pos.y].path[WEST]->y);
    printf("|");
    if (fast)
        printf(" fast");
    printf("\n");
}

