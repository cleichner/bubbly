#ifndef MOVEMENT_H
#define MOVEMENT_H

#include <stdbool.h>
#include <stdint.h>

typedef enum {
    FRONT,
    LEFT,
    RIGHT
} direction_t;

/* Any initialization that needs to happen in the main function. */
void initialize_movement(int argc, char* argv[]);
void finalize_movement(void);

/* Move n squares forward. */
void move_forward(int8_t n);

/* Rotate 90 degrees counter-clockwise. */
void rotate_left(void);

/* Rotate 90 degrees clockwise. */
void rotate_right(void);

/* Returns true if there is currently a wall in the specified direction with
 * respect to the robot. */
bool has_wall(direction_t direction);

#endif /* MOVEMENT_H */
