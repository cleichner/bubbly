#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

#include "maze.h"
#include "movement.h"

void init_maze(struct cell maze[WIDTH][HEIGHT]) {
    int8_t i;
    int8_t j;
    for (i = 0; i < WIDTH; i++) {
        for (j = 0; j < HEIGHT; j++) {
            maze[i][j].x = 0;
            maze[i][j].y = 0;
            maze[i][j].north = NULL;
            maze[i][j].south = NULL;
            maze[i][j].east = NULL;
            maze[i][j].west = NULL;
        }
    }
}

void connect_maze(struct cell maze[WIDTH][HEIGHT]) {
    int8_t i;
    int8_t j;
    for (i = 0; i < WIDTH; i++) {
        for (j = 0; j < HEIGHT; j++) {
            maze[i][j].x = i;
            maze[i][j].y = j;
            if (j != HEIGHT-1) {
                maze[i][j].north = &(maze[i][j+1]);
            }
            if (j != 0) {
                maze[i][j].south = &(maze[i][j-1]);
            }
            if (i != WIDTH-1) {
                maze[i][j].east = &(maze[i+1][j]);
            }
            if (i != 0) {
                maze[i][j].west = &(maze[i-1][j]);
            }
        }
    }
}

void print_maze(struct cell maze[WIDTH][HEIGHT]) {
    int8_t i;
    int8_t j;
    for (i = 0; i < WIDTH; i++) {
        for (j = 0; j < HEIGHT; j++) {
            printf("maze[%d][%d]\n", i, j);
            if (maze[i][j].north != NULL)
                printf("maze[%d][%d].north = (%d,%d)\n", i, j, maze[i][j].north->x,
                        maze[i][j].north->y);
            if (maze[i][j].south != NULL)
                printf("maze[%d][%d].south = (%d,%d)\n", i, j, maze[i][j].south->x,
                        maze[i][j].south->y);
            if (maze[i][j].east != NULL)
                printf("maze[%d][%d].east = (%d,%d)\n", i, j, maze[i][j].east->x,
                        maze[i][j].east->y);
            if (maze[i][j].west != NULL)
                printf("maze[%d][%d].west = (%d,%d)\n", i, j, maze[i][j].west->x,
                        maze[i][j].west->y);
            if (i != WIDTH-1 || j != HEIGHT-1) {
                printf("\n");
            }
        }
    }
}

int main(int argc, char* argv[]) {
    struct cell maze[WIDTH][HEIGHT];
    initialize_movements(argc, argv);
    init_maze(maze);
    connect_maze(maze);
//  print_maze(maze);
    return 0;
}
