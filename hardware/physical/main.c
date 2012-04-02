#include "encoder.h"
#include "global.h"
#include "motor.h"
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

//Digital ISR
ISR(PCINT2_vect){
    
    //Check to see which pin has changed by comparing the value actually on the pin
    //to the one stored in the respective variable. If there is a change, update current
    
    //Left motor
    if( (PIND & PIND0) != left_current_A){
        left_current_A = (PIND & PIND0);
        LEUF = true;
        EUF = true; 
    }
    if( (PIND & PIND1) != left_current_B){
        left_current_B = (PIND & PIND1);
        LEUF = true;
        EUF = true;
    }
    
    //Right motor
    if( (PIND & PIND5) != right_current_A){
        right_current_A = (PIND & PIND5);
        REUF = true;
        EUF = true;
    }
    if( (PIND & PIND6) != right_current_B){
        right_current_B = (PIND & PIND6);
        REUF = true;
        EUF = true;
    }
        
    //Encoder Update Flag
    sei();
    
}

//ADC ISR
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

//MAIN FUNCTION 
int main(void) {
    
    enc_init();
    motor_init();
    
    //encoder_debug_init();
    while (1){
        
        if (EUF) {
            update_encoder();
            //encoder_debug();
        }

    }
}
