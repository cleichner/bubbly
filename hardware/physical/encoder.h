//***************************************************************//
//
//File: encoder.h
//Author: Kevin Klug
//Date: 2/14/2012
//
//Description: The following contains functions needed to implement
//             quadrature encoding on an ATMEGA328
//
//***************************************************************//

#ifndef _encoder_h
#define _encoder_h

#include <avr/io.h>
#include "global.h"

//****************
//Useful Functions
//****************

//Configures the ATMEGA registers for correct usage of the
//PORTD interrupts.
void enc_init (void);

//Will find out which motors changed and in which direction
//Will also increment the global variables r_state and l_state
void update_encoder (void);

//Will turn on LED2 & LED3 based on the directions of the motors
void encoder_debug (void);

#endif

