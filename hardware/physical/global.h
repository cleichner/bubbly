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

//****************
//Global Variables
//****************

//ALL ENCODER DATA IS STORED IN ONE BYTE
//Data Stored as the following
//-R-|-L-
//--AB--AB  Current
//AB--AB--  Past
volatile uint8_t current_state;

//These variables will count how many times the motors have turned
volatile uint64_t r_state;
volatile uint64_t l_state;

//Encoder Update Flag
volatile uint8_t EUF;


#endif
