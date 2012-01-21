#ifndef MOVEMENT_H
#define MOVEMENT_H

#include <stdbool.h>

typedef enum {
    FRONT,
    LEFT,
    RIGHT,
} direction_t;

// Any initialization that needs to happen in the main function.
void initialize_movements(int argc, char* argv[]);

// Move one square forward.
void move_forward(void);

// Rotate to the 90 degrees counter-clockwise.
void rotate_left(void);

// Rotate to the 90 degrees clockwise.
void rotate_right(void);

// Returns true if there is currently a wall in the specified direction with
// respect to the robot.
bool has_wall(direction_t direction);

#endif // MOVEMENT_H
