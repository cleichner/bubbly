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

//*******************
//LOW LEVEL FUNCTIONS
//*******************

//The following preform bit operations to return the requested
//value that is stored in current_state
inline int8_t left_current_state (void){
    return (current_state & (PinD0+PinD1));
}
inline int8_t right_current_state (void){
    return (current_state & ((PinD5+PinD6)>>4));
}

inline int8_t left_past_state (void){
    return (current_state & ((PinD0+PinD1)>>2));
}
inline int8_t right_past_state (void){
    return (current_state & ((PinD5+PinD6)>>6));
}



#endif

