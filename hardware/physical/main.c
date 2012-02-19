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
    if( (PIND & PinD0) != left_current_A){
        left_current_A = (PIND & PinD0);
        LEUF = TRUE;
    }
    if( (PIND & PinD1) != left_current_B){
        left_current_B = (PIND & PinD1);
        LEUF = TRUE;
    }
    if( (PIND & PinD5) != right_current_A){
        right_current_A = ((PIND>>4)%4);
        REUF = TRUE;
    }
    if( (PIND & PinD6) != right_current_B){
        right_current_B = (PIND & PinD6)>>5;
        REUF = TRUE;
    }
    EUF = TRUE;
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
    //I'm not to sure what is going on here....
    if (motor_current_monitor == '0') motor_current_monitor = '1';
    else motor_current_monitor = '0';
    
    ADMUX = motor_current_monitor-'0';
    
    sei();
}

int main(void) {
    enc_init();
    motor_init();
    
    //Some debugging using LED2 and LED3 on the board
    //Configure Pins PC2 & PC3 as outputs
    DDRC |= _BV(2);
    DDRC |= _BV(3);
    //Set Pins PC2 & PC3 Low
    PORTC &= ~(_BV(2));
    PORTC &= ~(_BV(3));
    
    //This is Debugging stuff
    motor_set_direction('l', 'f');
    motor_set_direction('r', 'f');
    motor_set_speed('l',1);
    motor_set_speed('r',1);
    
    while (1){
        
        if (EUF) {
            update_encoder();
            encoder_debug();
        }
        
        motor_set_direction('l', 'f');
        motor_set_direction('r', 'f');
        motor_set_speed('l',5);
        motor_set_speed('r',5);
        _delay_ms(1000);
        
        if(EUF){
            update_encoder();
            encoder_debug();
        }
        motor_set_direction('l', 'r');
        motor_set_direction('r', 'r');
        _delay_ms(1000);
    }
}




