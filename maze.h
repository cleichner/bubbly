#ifndef MAZE_H
#define MAZE_H

#define WIDTH 5
#define HEIGHT 5

typedef enum {
    NORTH,
    SOUTH,
    EAST,
    WEST
} abs_direction;

struct cell {
    uint8_t x;
    uint8_t y;
    struct cell* north;
    struct cell* south;
    struct cell* east;
    struct cell* west;
};

void init_maze(struct cell maze[WIDTH][HEIGHT]);

#endif /* MAZE_H */
