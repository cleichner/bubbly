#ifndef ENCODER_H
#define ENCODER_H

// Configures the ATMEGA registers for correct usage of the
// PORTD interrupts.
void enc_init(void);

void update_encoder(void);

// Will turn on LED2 & LED3 based on the directions of the motors
void encoder_debug(void);

#endif // ENCODER_H

