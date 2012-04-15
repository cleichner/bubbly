//
//  motor.c
//  
//
//  Created by Kevin Klug on 2/18/12.
//
//  Description:
//      Controls two motors M1 & M1 on pins PB1 & PB2 using PWM. For more information go to
//      http://www.sparkfun.com/products/9571 .
//  Much Credit for this code goes to the Sparkfun Open source for
//  the Serial Controlled Dual Motor Controller

#include <stdio.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/sfr_defs.h>
#include "motor.h"
#include "global.h"


void motor_init (void)
{
    //Enabling PWM and SPI outputs
    DDRB = (1<<PWM1)|(1<<PWM2)|(1<<MOSI)|(1<<SCK);
    PORTB = (1<<MISO);//Enable MISO pull-up
        
    //Enable PWM using Timer 1 using 10 BIT Fast mode
    //Output toggle on OC1A and OC1B
    //
    //Set toggle mode for PWM pins and set 10bit fast PWM mode
    TCCR1A = (1<<COM1A1) | (1<<COM1B1) | (1<<WGM10) | (1<<WGM11);
    TCCR1B = (1<<WGM12);	//Set 8bit fast PWM mode
    OCR1A = 0;
	OCR1B = 0;
    TCCR1B |= (1<<CS12);
    
    //Motor Current Protection
    ADMUX = motor_current_monitor-'0';//ADC Channel 0
	ADCSRA = (1<<ADEN)|(1<<ADATE)|(1<<ADIE)|(1<<ADPS2)|(1<<ADPS1)|(1<<ADPS0);
    //Enable the ADC, auto-triggering, the ADC Interrupt and set the ADC prescaler to 64
	ADCSRA |= (1<<ADSC);	//Start the first conversion
}

void motor_set_direction( char motor, char direction){
    if (motor == 'r') {
        //Right motor is M1
        if (direction == 'f') {
            M1_REVERSE();
        }else{
            M1_FORWARD();
        }
    }else{
        //Left motor is M2
        if (direction == 'f') {
            M2_FORWARD();
        }else{
            M2_REVERSE();
        }
    }
}

void motor_set_speed( char motor, int speed){
    if (motor == 'r'){
        if (speed== 9) {
            OCR1A = 0x3FF;
        }else{
            OCR1A = SPEED_UNIT*speed;
        }
    }else{
        if (speed==9) {
            OCR1B = 0x3FF;
        }else{
            OCR1B = SPEED_UNIT*speed;
        }
    }
}
