#define F_CPU 16000000L
#include <avr/io.h>
#include "millis2560.h"

#define set_bit(value, bit)   (_SFR_BYTE(value) |= _BV(bit))
#define clear_bit(value, bit) (_SFR_BYTE(value) &= ~_BV(bit))

int main(void) {
    set_bit(DDRB, 7);
    clear_bit(PORTB, 7);

    uint8_t state = 0;

    millis2560_init();

    unsigned long time_previous, time_current;
    time_previous = millis2560();

    while (1) {
        time_current = millis2560();

        if ((time_current - time_previous) > 1000) {
            time_previous = time_current;

            state = !state;

            if (state)
                set_bit(PORTB, 7);
            else
                clear_bit(PORTB, 7);
        }
    }

    return 1;
}