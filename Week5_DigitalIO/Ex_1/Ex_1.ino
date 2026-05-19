#define F_CPU 16000000L
#include <avr/io.h>
#include <util/delay.h>

int main(void) {
    DDRE |= 0x38;
    DDRG |= 0x20;

    while (1) {
        PORTE = (PORTE & ~0x38) | 0x10;
        PORTG &= ~0x20;
        _delay_ms(1000);

        PORTE = (PORTE & ~0x38) | 0x20;
        PORTG &= ~0x20;
        _delay_ms(1000);

        PORTE &= ~0x38;
        PORTG |= 0x20;
        _delay_ms(1000);

        PORTE = (PORTE & ~0x38) | 0x08;
        PORTG &= ~0x20;
        _delay_ms(1000);
    }

    return 0;
}