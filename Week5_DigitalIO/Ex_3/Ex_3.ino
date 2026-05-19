#define F_CPU 16000000L
#include <avr/io.h>
#include <util/delay.h>


int main(void) {
    DDRE |= 0x38;
    DDRG |= 0x20;
    DDRJ &= ~0x02;

    uint8_t state_previous = 0, state_current;
    int count = 0;
    int direction = 0;

    while (1) {
        state_current = (PINJ >> 1) & 0x01;

        if (state_current == 1) {
            if (state_previous == 0) {
                direction = !direction;
                _delay_ms(50);
            }
        }
        state_previous = state_current;

        if (count == 0) {
            PORTE = 0x10;
            PORTG = 0x00;
        } else if (count == 1) {
            PORTE = 0x20;
            PORTG = 0x00;
        } else if (count == 2) {
            PORTE = 0x00;
            PORTG = 0x20;
        } else if (count == 3) {
            PORTE = 0x08;
            PORTG = 0x00;
        }

        _delay_ms(100);

        if (direction == 0) {
            count++;
            if (count > 3) count = 0;
        } else {
            count--;
            if (count < 0) count = 3;
        }
    }

    return 0;
}