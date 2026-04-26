#define F_CPU 16000000L
#include <avr/io.h>
#include <util/delay.h>
#include "UART0.h"
#include "ADC.h"

int main(void) {
    UART0_init();
    ADC_init(0, SINGLE_CONVERSION);

    DDRE |= (1 << PE4) | (1 << PE5) | (1 << PE3);
    DDRG |= (1 << PG5);

    while (1) {
        int read = read_ADC();
        int pos  = (read >> 8);

        PORTE &= ~((1 << PE4) | (1 << PE5) | (1 << PE3));
        PORTG &= ~(1 << PG5);

        switch (pos) {
            case 0: PORTE |= (1 << PE4); break; 
            case 1: PORTE |= (1 << PE5); break;
            case 2: PORTG |= (1 << PG5); break;
            case 3: PORTE |= (1 << PE3); break;
        }

        UART0_print(read);
        UART0_write(' ');
        UART0_print(pos);
        UART0_write('\n');
        _delay_ms(100);
    }
    return 0;
}
