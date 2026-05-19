#define F_CPU 16000000L
#include <avr/io.h>
#include <util/delay.h>
#include "UART0.h"

int main(void) {
    UART0_init();
    DDRJ &= ~0x02;

    uint8_t state_previous = 0, state_current;
    int count = 0;

    while (1) {
        state_current = (PINJ >> 1) & 0x01;

        if (state_current == 1 && state_previous == 0) {
            count++;
            
            UART0_print("Count is ");
            UART0_print(count);
            UART0_write('\n');
            
            _delay_ms(50);
        }
        state_previous = state_current;
    }

    return 0;
}
