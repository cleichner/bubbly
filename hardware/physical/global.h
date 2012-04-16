#ifndef GLOBAL_H
#define GLOBAL_H

#include <avr/io.h>
#include <stdbool.h>

#define FORWARD 1
#define REVERSE 0

// The following are global variables for the encoders
volatile uint8_t left_current_A;
volatile uint8_t left_current_B;
volatile uint8_t left_past_A;
volatile uint8_t left_past_B;

volatile uint8_t right_current_A;
volatile uint8_t right_current_B;
volatile uint8_t right_past_A;
volatile uint8_t right_past_B;
volatile uint8_t encoder_buff;

//These count the number of turns
volatile uint16_t right_turns;
volatile uint16_t left_turns;

// These indicate the last known direction
volatile uint8_t right_direction;
volatile uint8_t left_direction;

// These are flags to check which motor has tripped
volatile uint8_t EUF;
volatile uint8_t LEUF;
volatile uint8_t REUF;

// Port C Pin Assignments
#define SENSE_1	0
#define SENSE_2	1

// Port D Pin Assignments
#define	M1_P	2
#define M1_N	3
#define M2_P	4
#define M2_N	7

// Port B Pin Assignments
#define PWM1	1
#define PWM2	2

#endif // GLOBAL_H
