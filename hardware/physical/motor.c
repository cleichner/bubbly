//  Description:
//      Controls two motors M1 & M1 on pins PB1 & PB2 using PWM. For more
//      information go to http://www.sparkfun.com/products/9571 .
//  Much Credit for this code goes to the Sparkfun Open source for
//  the Serial Controlled Dual Motor Controller

#include <stdio.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/sfr_defs.h>
#include "motor.h"
#include "global.h"

void motor_init (void) {
    //Enabling PWM outputs
    DDRB = _BV(PWM1) | _BV(PWM2);

    //Enable PWM using Timer 1 using 10 BIT Fast mode
    //Output toggle on OC1A and OC1B
    //
    //Set toggle mode for PWM pins and set 10bit fast PWM mode
    TCCR1A = _BV(COM1A1) | _BV(COM1B1) | _BV(WGM10) | _BV(WGM11);
    TCCR1B = _BV(WGM12);	//Set 8bit fast PWM mode
    OCR1A = 0;
	OCR1B = 0;
    TCCR1B |= _BV(CS12);

    // Motor Current Protection
    ADMUX = 0; // ADC Channel 0

	ADCSRA = _BV(ADEN)  // ADC enable
           | _BV(ADIE)  // Interrupt enable
           | _BV(ADATE) // Enable Auto-triggering, free-runs by default

           | _BV(ADPS2) // 128 prescaler
           | _BV(ADPS1)
           | _BV(ADPS0)
           ;

    // Start the first conversion
	ADCSRA |= _BV(ADSC);
}

void motor_set_direction( char motor, char direction){
    if (motor == 'r') {
        //Right motor is M1
        if (direction == 'f') {
            M1_REVERSE();
        } else {
            M1_FORWARD();
        }
    } else {
        //Left motor is M2
        if (direction == 'f') {
            M2_FORWARD();
        } else {
            M2_REVERSE();
        }
    }
}

void motor_set_speed( char motor, int speed){
    if (motor == 'r') {
        if (speed == 9)  {
            OCR1A = 0x3FF;
        } else {
            OCR1A = SPEED_UNIT*speed;
        }
    } else {
        if (speed == 9) {
            OCR1B = 0x3FF;
        } else {
            OCR1B = SPEED_UNIT*speed;
        }
    }
}
