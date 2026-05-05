#define F_CPU 16000000L
#include <avr/io.h>
#include "millis2560.h"

#define set_bit(value, bit) ( _SFR_BYTE(value) |= _BV(bit) )
#define clear_bit(value, bit) ( _SFR_BYTE(value) &= ~_BV(bit) )

int main(void) {
    set_bit(DDRE, 4); clear_bit(PORTE, 4);
    set_bit(DDRE, 5); clear_bit(PORTE, 5);

    uint8_t state_pe4 = 0;
    uint8_t state_pe5 = 0;

    millis2560_init();

    unsigned long prev_pe4 = millis2560();
    unsigned long prev_pe5 = prev_pe4;
    unsigned long current_time;

    while (1) {
        current_time = millis2560();

        if ((current_time - prev_pe4) >= 500) {
            prev_pe4 = current_time;
            state_pe4 = !state_pe4;
            if (state_pe4) set_bit(PORTE, 4);
            else clear_bit(PORTE, 4);
        }

        if ((current_time - prev_pe5) >= 1000) {
            prev_pe5 = current_time;
            state_pe5 = !state_pe5;
            if (state_pe5) set_bit(PORTE, 5);
            else clear_bit(PORTE, 5);
        }
    }
    return 0;
}