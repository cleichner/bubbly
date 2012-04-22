#include "encoder.h"
#include "global.h"
#include "motor.h"
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

// from hardware.h (should be incorporated soon)
typedef enum {
    FRONT = 0,
    LEFT = 1,
    RIGHT = 2
} side_t;

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

volatile uint16_t sensor[3] = {0};
volatile uint16_t sensor_max[3] = {0};

// ADC ISR
ISR (ADC_vect) {
    static bool first = true;
    if (first) {
        first = false;
    } else {
        if ((((ADMUX & 0x0F) == 0) || ((ADMUX & 0xF) == 1)) &&
            (ADC > CURRENT_THRESHOLD)) {
            // should signal motor failure
            motor_set_speed( 'r', 0);
            motor_set_speed( 'l', 0);
        } else if ((ADMUX & 0x0F) == 4) {
            sensor[FRONT] = ADC;
            if (sensor[FRONT] > sensor_max[FRONT])
                sensor_max[FRONT] = sensor[FRONT];
        } else if ((ADMUX & 0x0F) == 7) {
            sensor[RIGHT] = ADC;
            if (sensor[RIGHT] > sensor_max[RIGHT])
                sensor_max[RIGHT] = sensor[RIGHT];
        } else if ((ADMUX & 0x0F) == 6) {
            sensor[LEFT] = ADC;
            if (sensor[LEFT] > sensor_max[LEFT])
                sensor_max[LEFT] = sensor[LEFT];
        } else {
            // should signal failure
        }

        uint8_t X = 0;
        uint8_t next_sensor[] = {1, 4, X, X, 6, X, 7, 0};
        ADMUX = (ADMUX & 0xF0) | next_sensor[ADMUX & 0x0F];
        first = true;
    }

	ADCSRA |= _BV(ADSC);
}

inline float norm(side_t dir) {
    return (float) sensor[dir] / (float) sensor_max[dir];
}

void kill_motors(void) {
    motor_set_speed('r', 0);
    motor_set_speed('l', 0);
}

// Move n squares forward.
void move_forward(int8_t n) {
    uint8_t i;
    motor_set_direction('r', 'f');
    motor_set_direction('l', 'f');
    for (i = 0; i < n; i++) {
        right_turns = 0;
        left_turns = 0;
        while ((right_turns < 800 || left_turns < 800) && (PINC & _BV(PINC5))) {
            uint8_t dir = norm(LEFT) > norm(RIGHT) ? LEFT : RIGHT;
            float error = 0.5 - norm(dir);
            float k_p = 800;
            if (error > 0) {
                OCR1A = (int16_t) k_p * error;
                OCR1B = 1023 - OCR1A;
                PORTC |= _BV(PORTC3);
            } else {
                error = -error;
                OCR1B = (int16_t) k_p * error;
                OCR1A = 1023 - OCR1B;
                PORTC &= ~_BV(PORTC3);
            }

            error = right_turns - left_turns;
            k_p = 0.5;
            if (error > 0) {
                OCR1B -= k_p * error;
                OCR1A += k_p * error;
            } else {
                error = -error;
                OCR1A -= k_p * error;
                OCR1B += k_p * error;
            }
        }
    }
    kill_motors();
}

// Rotate 90 degrees counter-clockwise n times.
void turn_left(int8_t n) {
    motor_set_direction('r', 'f');
    motor_set_direction('l', 'r');
    OCR1B = 100;
    OCR1A = 100;
    left_turns = 0;
    while (left_turns < 200) {
        OCR1B += 100;
        OCR1A += 100;
    }
    left_turns = 0;
    kill_motors();
}

// Rotate 90 degrees clockwise n times.
void turn_right(int8_t n) {
    motor_set_direction('l', 'f');
    motor_set_direction('r', 'r');
    OCR1B = 100;
    OCR1A = 100;
    right_turns = 0;
    while (right_turns < 200) {
        OCR1B += 100;
        OCR1A += 100;
    }
    right_turns = 0;
    kill_motors();
}

// Returns true if there is currently a wall in the specified direction with
// respect to the robot.
bool has_wall(side_t side) {
    return (norm(side) >= 0.4);
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

    //Enable all interrupts
    sei();

    DDRC &= ~(_BV(DDC5));
    while (!(has_wall(RIGHT) && has_wall(RIGHT))) {
        move_forward(1);
    }
    turn_right(1);
    move_forward(1);

    while (1) {
        if (has_wall(RIGHT)) {
            PORTC |= _BV(PORTC3);
        } else {
            PORTC &= ~(_BV(PORTC3));
        }
        if (has_wall(LEFT)) {
            PORTC |= _BV(PORTC2);
        } else {
            PORTC &= ~(_BV(PORTC2));
        }
        /*
        while ((PINC & _BV(PINC5)));
        move_forward(1);
        move_forward(1);
        turn_right(1);
        move_forward(1);
        */
    }
}
