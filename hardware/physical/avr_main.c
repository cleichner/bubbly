#include "global.h"
#include "encoder.h"

int main(void) {
    enc_init();
    while (1) {
        if (EUF) {
             //update_encoder();
            update_encoder();
        }
    }
    return 1;
}
