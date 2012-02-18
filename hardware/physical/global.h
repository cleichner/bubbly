//
//  global.h
//  
//
//  Created by Kevin Klug on 2/14/12.
//  Copyright (c) 2012 Univeristy of Arizona. All rights reserved.
//

#ifndef _global_h
#define _global_h

#include <avr/io.h>


//Left Motor
#define PinD0 0x01 //A
#define PinD1 0x02 //B

//Right Motor
#define PinD5 0x10 //A 
#define PinD6 0x20 //B 

volatile uint8_t left_current_A;
volatile uint8_t left_current_B;
volatile uint8_t left_past_A;
volatile uint8_t left_past_B;

volatile uint8_t right_current_A;
volatile uint8_t right_current_B;
volatile uint8_t right_past_A;
volatile uint8_t right_past_B;

volatile uint8_t right_turns;
volatile uint8_t left_turns;

volatile uint8_t EUF;



#endif
