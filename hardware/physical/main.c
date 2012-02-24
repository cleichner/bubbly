#include "encoder.h"
#include "global.h"
#include "motor.h"
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

//*****************//
//Digital ISR
//*****************//
ISR(PCINT2_vect){
    
    //Check to see which pin has changed by comparing the value actually on the pin
    //to the one stored in the respective variable. If there is a change, update current
    
    //Left motor
    if( (PIND & PinD0) != left_current_A){
        left_current_A = (PIND & PinD0);
        LEUF = TRUE;
        EUF = TRUE; 
    }
    if( (PIND & PinD1) != left_current_B){
        left_current_B = (PIND & PinD1);
        LEUF = TRUE;
        EUF = TRUE;
    }
    
    //Right motor
    if( (PIND & PinD5) != right_current_A){
        right_current_A = (PIND & PinD5);
        REUF = TRUE;
        EUF = TRUE;
    }
    if( (PIND & PinD6) != right_current_B){
        right_current_B = (PIND & PinD6);
        REUF = TRUE;
        EUF = TRUE;
    }
    
    //EUF = TRUE;
    
    //Encoder Update Flag
    sei();
    
}

//****************//
//ADC ISR
//****************//
ISR (ADC_vect){
    cli();
    
    current = ADCL;
    current |= ADCH<<8;
    
    if (current > CURRENT_THRESHOLD) {
        //Stop all motors
        motor_set_speed( 'r', 0);
        motor_set_speed( 'l', 0);
    }
    if (motor_current_monitor == '0') motor_current_monitor = '1';
    else motor_current_monitor = '0';
    
    ADMUX = motor_current_monitor-'0';
    
    sei();
}


//***************//
//MAIN FUNCTION  //
//***************//
int main(void) {
    
    enc_init();
    motor_init();
    
    //Some debugging using LED2 and LED3 on the board
    //Configure Pins PC2 & PC3 as outputs
    DDRC |= _BV(2);
    DDRC |= _BV(3);
    
    
    //This is Debugging stuff
    motor_set_direction('l', 'r');
    motor_set_direction('r', 'r');
    motor_set_speed('l',3);
    motor_set_speed('r',3);
    
    PORTC &= ~(_BV(2));
    PORTC &= ~(_BV(3));
    
    while (1){
        
        if (EUF) {
            update_encoder();
            encoder_debug();
        }

    }
}




