#define F_CPU 16000000L

#include <avr/io.h>
#include <util/delay.h>

int main(void)
{
    DDRE |= 0x30;

    PORTE &= ~0x10;
    PORTE |= 0x20;

    while (1)
    {
        PORTE ^= 0x30;
        _delay_ms(500);
    }

    return 0;
}