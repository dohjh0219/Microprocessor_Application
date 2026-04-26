#define F_CPU 16000000L
#include <avr/io.h>
#include <util/delay.h>
#include "UART0.h"
#include "ADC.h"

int main(void) {
    UART0_init();
    ADC_init(0, SINGLE_CONVERSION);

    DDRB |= (1 << PB7);

    while (1) {
        int read = read_ADC(); 

        int delay_ms = 200 + ((long)read * 1800L / 1023L);

        PORTB ^= (1 << PB7);

        UART0_print(read);
        UART0_write(' ');
        UART0_print(delay_ms);
        UART0_write('\n');

        for (int i = 0; i < delay_ms; i++) {
            _delay_ms(1);
        }
    }

    return 0;
}