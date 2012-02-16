#ifndef HARDWARE_H
#define HARDWARE_H

#include <stdbool.h>
#include <stdint.h>

#include "maze.h"

typedef enum {
    FRONT = 0,
    LEFT = 1,
    RIGHT = 2
} side_t;

// Any initialization that needs to happen in the main function.
void initialize_hardware(int argc, char* argv[]);

// Any cleanup that the hardware needs to do on exit
void finalize_hardware(void);

// Move n squares forward.
void move_forward(int8_t n);

// Rotate 90 degrees counter-clockwise n times.
void turn_left(int8_t n);

// Rotate 90 degrees clockwise n times.
void turn_right(int8_t n);

// Returns true if there is currently a wall in the specified direction with
// respect to the robot.
bool has_wall(side_t side);

direction_t fast_execute_actions(struct action actions[ACTION_SIZE]);

#endif // HARDWARE_H
