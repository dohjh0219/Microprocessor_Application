#define F_CPU 16000000L
#include <avr/io.h>
#include <avr/interrupt.h>

#define set_bit(value, bit)   ( _SFR_BYTE(value) |= _BV(bit) )
#define clear_bit(value, bit) ( _SFR_BYTE(value) &= ~_BV(bit) )

int main(void) {
    set_bit(DDRB, 7);
    clear_bit(PORTB, 7);

    TCCR0B |= (1 << CS02) | (1 << CS00);

    TIMSK0 |= (1 << OCIE0A);
    OCR0A = 255;

    TCCR0A |= (1 << COM0A0);

    TCCR0A |= (1 << WGM01);

    sei();

    while (1) {
    }

    return 0;
}