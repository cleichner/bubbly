//***************************************************************//
//
//File: encoder.c
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



//Configures the ATMEGA registers for correct usage of the
//PORTD interrupts.
void enc_init(void) {
    
    //Enable Serial Debugging
    //Note: This means only the right motor can be debugged as the left one
    //      uses PD0(RX) and PD1(TX)
    //Serial.begin(9600);
    //Serial.println("Initalizing Serial");
    
    //Configure Pins PD5 & PD6 as inputs by setting (0)
    DDRD &= ~(1<<5);
    DDRD &= ~(1<<6);
    
    //Enable Interrupt Procedure
    PCMSK2 |= (1<<PCINT21);
    //A logic change on PCINT21 will enable interrupt to PCINT2_vect
    PCMSK2 |= (1<<PCINT22);
    PCICR |= (1<<PCIE2);
    
    //Enable all interrupts
    sei();
    
}


void update_encoder (void){
    //Reset flag
    EUF = 0;
    cli();
    //Now find out which motors changed and in which direction
    //using the following coding
    
    //Encoding Table
    //__________________
    //Clockwise Rotation
    //Phase A    B   Return
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
    switch(right_current_A){
        case 0:
            if(!right_past_B) right_turns++;
            else right_turns--;
            break;
            
        case 1:
            if(right_past_B) right_turns++;
            else right_turns--;
            break;
            
        default:
            break;
    }
    //Update the past
    right_past_A = right_current_A;
    right_past_B = right_current_B;
    
    switch(left_current_A){
        case 0:
            if(!left_past_B) left_turns++;
            else left_turns--;
            break;
            
        case 1:
            if(left_past_B) left_turns++;
            else left_turns--;
            break;
            
        default:
            break;
    }
    left_past_A = left_current_A;
    left_past_B = left_current_B;
    sei();
    
}
