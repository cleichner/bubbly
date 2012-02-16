//***************************************************************//
//
//File: main.c
//Author: Kevin Klug
//Date: 2/14/2012
//
//Description: The following code will count the phase of two quadrature
//             encoded motors that are connected to an ATMEGA168
//             microcontroller. It stores its phase as well as the
//             number of turns in global variables which are accessed
//             in ISR
//
//***************************************************************//

#include "encoder.h"
#include "global.h"
#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdint.h>

//*****************//
//Digital ISR
//*****************//
ISR (PCINT2_vect) {
    //This interrupt will be called when PCINT(16,17,21,22) are changed
    if ((PORTD & PinD0) != (current_state & PinD0))
        current_state ^= PinD0;
    if ((PORTD & PinD1) != (current_state & PinD1))
        current_state ^= PinD1;
    if ((PORTD & PinD5) != (current_state & PinD5))
        current_state ^= PinD5;
    if ((PORTD & PinD6) != (current_state & PinD6)) 
        current_state ^= PinD6;    
    EUF++;
}

//Configures the ATMEGA registers for correct usage of the
//PORTD interrupts.
void enc_init(void) {
    //Set Encoder pins as inputs
    DDRD = DDRD & 0x1C;
    //Write Logic LOW to pins (HIGH-Z)
    PORTD = PORTD & 0x1C;
    
    //Enable Interrupt to PCIN2_vect for PCINT16,17,21,22    
    //Mask all others
    PCMSK2 = 0x63;
    PCICR = 0x04;
    
}

void update_encoder(void) {
    //Reset flag
    EUF = 0;
    
    //Now find out which motors changed and in which direction
    //using the following coding
    
    //Encoding Table
    //__________________
    //Clockwise Rotation
    //Phase    A    B   Return
    //1     0    0   0
    //2     0    1   1
    //3     1    1   3
    //4     1    0   2
    //_________________________
    //CounterClockwise Rotation
    //Phase    A    B   Return
    //1     1    0   2
    //2     1    1   3
    //3     0    1   1
    //4     0    0   0
    
    //Logic Table for left motor
    switch (left_current_state()) {
        case 0:
            if (left_past_state() == 2) {
                //CW rotation
                l_state++;
            } else {
                //CCW Rotation
                l_state--;
            }break;
            
        case 1:
            if (left_past_state() == 0) {
                l_state++;
            } else {
                l_state--;
            }break;
            
        case 2:
            if (left_past_state() == 3){
                l_state++;
            } else {
                l_state--;
            }break;
            
        case 3:
            if (left_past_state() == 1) {
                l_state++;
            } else {
                l_state--;
            }break;
            
        default:
            break;
    }
    
    //Logic Table for right motor
    switch (right_current_state()) {
        case 0:
            if (right_past_state() == 2) {
                //CW rotation
                r_state++;
            } else {
                //CCW Rotation
                r_state--;
            }break;
        case 1:
            if (right_past_state() == 0) {
                r_state++;
            } else {
                r_state--;
            }break;
        case 2:
            if (right_past_state() == 3){
                r_state++;
            } else {
                r_state--;
            }break;
            
        case 3:
            if (right_past_state() == 1) {
                r_state++;
            } else {
                r_state--;
            } break;
            
        default:
            break;
    }
    
    EUF = 0;
}


