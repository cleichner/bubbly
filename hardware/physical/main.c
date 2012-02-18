#include "encoder.h"
#include "global.h"
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

//*****************//
//Digital ISR
//*****************//
ISR(PCINT2_vect){
    //Check to see which pin has changed by comparing the value actually on the pin
    //to the one stored in the respective variable. If there is a change, update current
    if( (PIND & PinD0) != left_current_A) left_current_A = (PIND & PinD0);
    if( (PIND & PinD1) != left_current_B) left_current_B = (PIND & PinD1);
    if( (PIND & PinD5) != right_current_A) right_current_A = (PIND & PinD5)>>4;
    if( (PIND & PinD6) != right_current_B) right_current_B = (PIND & PinD6)>>5;
    EUF++;
}

int main(void) {
    enc_init();
    while (1){
        if (EUF) {
            update_encoder();
        }
    }
}

