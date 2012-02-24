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



//Configures the ATMEGA328p registers for correct usage of the
//PORTD interrupts.
void enc_init(void) {
    
    //Note: This means only the right motor can be debugged as the left one
    //      uses PD0(RX) and PD1(TX)
    
    //Configure Pins PD0,PD1,PD5,PD6 as inputs by setting (0)
    DDRD &= ~(1<<5);
    DDRD &= ~(1<<6);
    DDRD &= ~(1<<0);
    DDRD &= ~(1<<1);
    
    //Enable Interrupt Procedure
    //The following pins will trigger an interrupt at vector
    //PCINT2_vect
    PCMSK2 |= (1<<PCINT21);
    PCMSK2 |= (1<<PCINT22);
    PCMSK2 |= (1<<PCINT16);
    PCMSK2 |= (1<<PCINT17);
    PCICR |= (1<<PCIE2);
    
    right_direction = FORWARD;
    left_direction = FORWARD;
    
    //Enable all interrupts
    sei();
    
}


void update_encoder (void){
    //Reset flag
    EUF = FALSE;
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
    if (REUF == TRUE) {
        REUF = FALSE;
        switch(right_current_A){
            case 0:
                if(!right_past_B){
                    right_turns++;
                    right_direction = FORWARD;
                }
                else{
                    right_turns--;
                    right_direction = REVERSE;
                }
                break;
                
            case 1:
                if(right_past_B){
                    right_turns++;
                    right_direction = FORWARD;
                }else{
                    right_turns--;
                    right_direction = REVERSE;
                }
                break;
                
            default:
                break;
        }
        //Update the past
        right_past_A = right_current_A;
        right_past_B = right_current_B;
    }
    
    if (LEUF == TRUE) {
        LEUF = FALSE;
        switch(left_current_A){
            case 0:
                if(!left_past_B){
                    left_turns++;
                    left_direction = FORWARD;
                }else{
                    left_turns--;
                    left_direction = REVERSE;
                }
                break;
                
            case 1:
                if(left_past_B){
                    left_turns++;
                    left_direction = FORWARD;
                }else{
                    left_turns--;
                    left_direction = REVERSE;
                }
                break;
                
            default:
                break;
        }
        //update the past
        left_past_A = left_current_A;
        left_past_B = left_current_B;
    }
    //Enable Interrupts
    sei();
}

void encoder_debug (void){
    
    EUF = FALSE;
    
    //If the right motor is going forward
    //Turn on LED2
    if (right_direction == FORWARD) {
        PORTC |= _BV(2);
    }else{
        PORTC &= ~(_BV(2));
    }
    
    //If the left motor is going forward
    //Turn on LED3
    if (left_direction == FORWARD) {
        PORTC |= _BV(3);
    }else{
        PORTC &= ~(_BV(3));
    }
    
}

