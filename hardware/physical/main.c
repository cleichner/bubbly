#include "encoder.h"
#include "global.h"
#include "motor.h"
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

// Pin Change ISR
ISR(PCINT2_vect){
    // Check to see which pin has changed by comparing the value actually on
    // the pin to the one stored in the respective variable. If there is
    // a change, update current

    // Left motor
    encoder_buff = (PIND & _BV(PIND0));
    if (encoder_buff != left_current_A) {
        left_current_A = encoder_buff;
        LEUF = true;
        EUF = true;
    }

    encoder_buff = (PIND & _BV(PIND1));
    if (encoder_buff != left_current_B) {
        left_current_B = encoder_buff;
        LEUF = true;
        EUF = true;
    }

    // Right motor
    encoder_buff = (PIND & _BV(PIND6));
    if (encoder_buff != right_current_A) {
        right_current_A = encoder_buff;
        REUF = true;
        EUF = true;
    }
    encoder_buff = (PIND & _BV(PIND5));
    if (encoder_buff != right_current_B) {
        right_current_B = encoder_buff;
        REUF = true;
        EUF = true;
    }

    update_encoder();
    encoder_debug();
}

// ADC ISR
ISR (ADC_vect) {
    static uint8_t motor = 0;

    // Stop the motors on over-current
    if (ADC > CURRENT_THRESHOLD) {
        motor_set_speed( 'r', 0);
        motor_set_speed( 'l', 0);
    }

    ADMUX = motor ? 0 : 1;
}


void move_forward(int8_t n) {
    int8_t i;
    motor_set_direction('r', 'f');
    motor_set_direction('l', 'f');
    motor_set_speed('r', 9);
    motor_set_speed('l', 9);
    for (i = 0; i < n; i++) {
        while (right_turns < 500);
        right_turns = 0;
    }
    motor_set_speed('r', 0);
    motor_set_speed('l', 0);
}

int main(void) {
    enc_init();
    motor_init();

    encoder_debug_init();

    right_turns = 0;
    left_turns = 0;
    right_direction = REVERSE;
    left_direction = REVERSE;

    encoder_debug_init();

    DDRC &= ~(_BV(DDC5));
    uint8_t sensor_status = (PINC &_BV(PINC5));
    uint8_t sensor_poll;

    move_forward(3);
    while (1){
        /*
        if (EUF) {
            update_encoder();
            encoder_debug();
        }
        sensor_poll = PINC & _BV(PINC5);
        if(sensor_poll != sensor_status) {
            sensor_status = sensor_poll;
            if (right_direction == FORWARD) {
                motor_set_direction('l', 'r');
                motor_set_direction('r', 'r');
            } else {
                motor_set_direction('l', 'f');
                motor_set_direction('r', 'f');
            }
        }
        */
    }
}
