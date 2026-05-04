#define F_CPU 16000000L
#include <avr/io.h>
#include <avr/interrupt.h>

#define set_bit(value, bit) ( _SFR_BYTE(value) |= _BV(bit) )
#define clear_bit(value, bit) ( _SFR_BYTE(value) &= ~_BV(bit) )

volatile int count0 = 0;
volatile int count2 = 0;

uint8_t state_pe4 = 0;
uint8_t state_pe5 = 0;

ISR(TIMER0_COMPA_vect) {
    count0++;
    TCNT0 = 0;
}

ISR(TIMER2_COMPA_vect) {
    count2++;
    TCNT2 = 0;
}

int main(void) {
    // PE4, PE5를 출력으로 설정
    set_bit(DDRE, 4); clear_bit(PORTE, 4);
    set_bit(DDRE, 5); clear_bit(PORTE, 5);

    // 타이머 0 설정 (분주비 1024) [cite: 185]
    TCCR0B |= (1 << CS02) | (1 << CS00);
    TIMSK0 |= (1 << OCIE0A);
    OCR0A = 128;

    // 타이머 2 설정 (분주비 1024) [cite: 593]
    TCCR2B |= (1 << CS22) | (1 << CS21) | (1 << CS20);
    TIMSK2 |= (1 << OCIE2A);
    OCR2A = 128;

    sei();

    while (1) {
        // 타이머 0 (0.5초 제어)
        if (count0 >= 64) {
            count0 = 0;
            state_pe4 = !state_pe4;
            if (state_pe4) set_bit(PORTE, 4);
            else clear_bit(PORTE, 4);
        }

        // 타이머 2 (1.0초 제어, 64 * 2 = 128회)
        if (count2 >= 128) {
            count2 = 0;
            state_pe5 = !state_pe5;
            if (state_pe5) set_bit(PORTE, 5);
            else clear_bit(PORTE, 5);
        }
    }
    return 0;
}