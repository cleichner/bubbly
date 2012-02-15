#ifndef MAZE_H
#define MAZE_H

#include <stdint.h>
#include <stdbool.h>

#define WIDTH 8
#define HEIGHT 8

typedef enum {
    NORTH = 0,
    SOUTH = 1,
    EAST = 2,
    WEST = 3
} direction_t;

struct cell {
    uint8_t x;
    uint8_t y;
    struct cell* path[4]; // NORTH, SOUTH, EAST, WEST
    bool visited;
};

struct point {
    int8_t x;
    int8_t y;
};

struct action {
    void (*move)(int8_t n);
    int8_t times;
};

void init_maze(struct cell maze[WIDTH][HEIGHT]);

#endif // MAZE_H
