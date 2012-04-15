#include "encoder.h"
#include "global.h"
#include "motor.h"
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

// Pin Change ISR
ISR(PCINT2_vect) {
    //Check to see which pin has changed by comparing the value actually on the
    //pin to the one stored in the respective variable. If there is a change,
    //update current

    //Left motor
    if ((PIND & _BV(PIND0)) != left_current_A){
        left_current_A = (PIND & PIND0);
        LEUF = true;
        EUF = true;
    }
    if ((PIND & _BV(PIND1)) != left_current_B){
        left_current_B = (PIND & PIND1);
        LEUF = true;
        EUF = true;
    }

    //Right motor
    if ((PIND & _BV(PIND5)) != right_current_A){
        right_current_A = (PIND & PIND5);
        REUF = true;
        EUF = true;
    }
    if ((PIND & _BV(PIND6)) != right_current_B){
        right_current_B = (PIND & PIND6);
        REUF = true;
        EUF = true;
    }
}

// ADC ISR
ISR(ADC_vect) {
    static uint8_t motor;

    if (ADC > CURRENT_THRESHOLD) {
        //Stop all motors
        motor_set_speed( 'r', 0);
        motor_set_speed( 'l', 0);
    }

    ADMUX = motor ? 0 : 1;
}

int main(void) {
    enc_init();
    motor_init();

    //Some debugging using LED2 and LED3 on the board
    //Configure Pins PC2 & PC3 as outputs
    DDRC |= _BV(PORTC2);
    DDRC |= _BV(PORTC3);

    //This is Debugging stuff
    motor_set_direction('l', 'f');
    motor_set_direction('r', 'b');
    motor_set_speed('l',9);
    motor_set_speed('r',9);

    PORTC &= ~(_BV(PORTC2));
    PORTC &= ~(_BV(PORTC3));

    while (1){
        if (EUF)
            update_encoder();
    }
}
