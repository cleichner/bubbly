#ifndef ENCODER_H
#define ENCODER_H

// Configures the ATMEGA registers for correct usage of the
// PORTD interrupts.
void enc_init(void);

//Will find out which motors changed and in which direction
void update_encoder(void);

// Will turn on LED2 & LED3 based on the directions of the motors
void encoder_debug(void);

void encoder_debug_init (void);

#endif // ENCODER_H
