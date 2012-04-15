#ifndef MOTOR_H
#define MOTOR_H

//MACROS
#define sbi(var, mask)   ((var) |= (uint8_t)(1 << mask))
#define cbi(var, mask)   ((var) &= (uint8_t)~(1 << mask))

#define M1_FORWARD()	PORTD |= _BV(M1_P);PORTD &= ~(_BV(M1_N))
#define M1_REVERSE()	PORTD |= _BV(M1_N);PORTD &= ~(_BV(M1_P))

#define M2_FORWARD()	PORTD |= _BV(M2_P);PORTD &= ~(_BV(M2_N))
#define M2_REVERSE()	PORTD |= _BV(M2_N);PORTD &= ~(_BV(M2_P))

#define SPEED_UNIT	114
#define AVERAGE	16

// This is compared against the 10 bit ADC value and corresponds to roughly
// 1.5A on the Current Sense pin from the Motor Controller
#define CURRENT_THRESHOLD	150

void motor_init (void); //Initalizes Pins for PWM and ADC needed for ADC
void motor_set_direction( char motor, char direction);
void motor_set_speed(char motor, int speed);

#endif
