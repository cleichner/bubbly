#ifndef MOVEMENT_H
#define MOVEMENT_H

#include <stdbool.h>

typedef enum{
    FRONT,
    LEFT,
    RIGHT,
} direction_t;

/* moves the robot one square forward*/
void move_forward(void);

/* rotate to the 90 degrees counter-clockwise */
void rotate_left(void);

/* rotate to the 90 degrees clockwise */
void rotate_right(void);

/* respond if there is currently a wall in the specified direction with 
 * respect to the robot
 */
bool has_wall(direction_t direction);

#endif
