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
    DDRD &= ~(1<<PORTD0);
    DDRD &= ~(1<<PORTD1);
    DDRD &= ~(1<<PORTD5);
    DDRD &= ~(1<<PORTD6);

    
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
    EUF = false;
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
    if (REUF == true) {
        REUF = false;
        switch(right_current_A){
            case 0:
                if(!right_past_B){
                    if (right_direction == REVERSE) {
                        right_direction = FORWARD;
                        right_turns = 0;
                    }
                    right_turns++;
                }
                else{
                    if (right_direction == FORWARD) {
                        right_direction = REVERSE;
                        right_turns = 0;
                    }
                    right_turns--;
                }
                break;
                
            case 1:
                if(right_past_B){
                    if (right_direction == REVERSE) {
                        right_direction = FORWARD;
                        right_turns = 0;
                    }
                    right_turns++;
                }else{
                    if (right_direction == FORWARD) {
                        right_direction = REVERSE;
                        right_turns = 0;
                    }
                    right_turns--;
                }
                break;
                
            default:
                break;
        }
        //Update the past
        right_past_A = right_current_A;
        right_past_B = right_current_B;
    }
    
    if (LEUF == true) {
        LEUF = false;
        switch(left_current_A){
            case 0:
                if(!left_past_B){
                    //If the direction changed reset number
                    //of turns.
                    if (left_direction == REVERSE) {
                        left_direction = FORWARD;
                        left_turns = 0;
                    }
                    left_turns++;
                }else{
                    if (left_direction == FORWARD) {
                        left_direction = REVERSE;
                        left_turns = 0;
                    }
                    left_turns--;
                }
                break;
                
            case 1:
                if(left_past_B){
                    if (left_direction == REVERSE) {
                        left_direction = FORWARD;
                        left_turns = 0;
                    }
                    left_turns++;
                }else{
                    if (left_direction == FORWARD) {
                        left_direction = REVERSE;
                        left_turns = 0;
                    }
                    left_turns--;
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

//Some debugging using LED2 and LED3 on the board
void encoder_debug_init (void){
    //Configure Pins PC2 & PC3 as outputs
    DDRC |= _BV(PORTC2);
    DDRC |= _BV(PORTC3);
    
    
    //This is Debugging stuff
    motor_set_direction('l', 'r');
    motor_set_direction('r', 'r');
    motor_set_speed('l',3);
    motor_set_speed('r',3);
    
    PORTC &= ~(_BV(PORTC2));
    PORTC &= ~(_BV(PORTC3));
}

//For this to work encoder_debug_init must be run prior!
void encoder_debug (void){
    
    EUF = false;
    
    //If the right motor is going forward
    //Turn on LED2
    if (right_direction == FORWARD) {
        PORTC |= _BV(PORTC2);
    }else{
        PORTC &= ~(_BV(PORTC2));
    }
    
    //If the left motor is going forward
    //Turn on LED3
    if (left_direction == FORWARD) {
        PORTC |= _BV(PORTC3);
    }else{
        PORTC &= ~(_BV(PORTC3));
    }
    
}
