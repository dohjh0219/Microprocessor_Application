#define F_CPU 16000000L
#include <avr/io.h>
#include <avr/interrupt.h>

#define set_bit(value, bit) ( _SFR_BYTE(value) |= _BV(bit) )
#define clear_bit(value, bit) ( _SFR_BYTE(value) &= ~_BV(bit) )

volatile int count = 0;
uint8_t state = 0;

ISR(TIMER2_COMPA_vect) {
    count++;
    TCNT2 = 0;
}

int main(void) {
    set_bit(DDRB, 7); 
    clear_bit(PORTB, 7);

    TCCR2B |= (1 << CS22) | (1 << CS21) | (1 << CS20);
    TIMSK2 |= (1 << OCIE2A);
    OCR2A = 128; 
    sei();

    while (1) {
        if (count == 64) { 
            count = 0;
            state = !state;
            if (state) set_bit(PORTB, 7);
            else clear_bit(PORTB, 7);
        }
    }
    return 0;
}