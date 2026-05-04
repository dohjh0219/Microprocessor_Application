#define F_CPU 16000000L
#include <avr/io.h>
#include <avr/interrupt.h>

#define set_bit(value, bit) ( _SFR_BYTE(value) |= _BV(bit) )
#define clear_bit(value, bit) ( _SFR_BYTE(value) &= ~_BV(bit) )

volatile int count = 0; 
uint8_t state = 0;

ISR(TIMER0_COMPA_vect) {
    // TCNT0 = 0; 구문 생략 (CTC 모드라 자동 초기화 됨) 
    count++;
    
    if (count == 64) {
        count = 0;
        state = !state;
        if (state) set_bit(PORTE, 4);
        else clear_bit(PORTE, 4);
    }
}

int main(void) {
    set_bit(DDRB, 7); 
    set_bit(DDRE, 4); 
    
    clear_bit(PORTB, 7);
    clear_bit(PORTE, 4);

    TCCR0B |= (1 << CS02) | (1 << CS00); 
    TIMSK0 |= (1 << OCIE0A); 
    OCR0A = 128; 

    TCCR0A |= (1 << COM0A0); // PB7 하드웨어 자동 반전 유지 [cite: 407]
    TCCR0A |= (1 << WGM01);  // CTC 모드 활성화 

    sei(); 

    while (1) {
        // 메인 루프는 비워둠
    }
    return 0;
}