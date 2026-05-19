#define F_CPU 16000000L

#include <avr/io.h>
#include <util/delay.h>

#define set_bit(value, bit)   (_SFR_BYTE(value) |= _BV(bit))
#define clear_bit(value, bit) (_SFR_BYTE(value) &= ~_BV(bit))

int main(void)
{
    DDRJ &= ~0x01;
    DDRH &= ~0x02;

    DDRE |= 0x20;
    DDRG |= 0x20;

    while (1)
    {
        uint8_t btn_pulldown = PINJ & 0x01;
        uint8_t btn_pullup   = (PINH >> 1) & 0x01;

        if (btn_pulldown)
            set_bit(PORTE, 5);
        else
            clear_bit(PORTE, 5);

        if (btn_pullup)
            set_bit(PORTG, 5);
        else
            clear_bit(PORTG, 5);
    }

    return 0;
}