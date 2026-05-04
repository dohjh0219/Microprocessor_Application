#define F_CPU 16000000L
#include <avr/io.h>
#include <avr/interrupt.h>

#define set_bit(value, bit) ( _SFR_BYTE(value) |= _BV(bit) )
#define clear_bit(value, bit) ( _SFR_BYTE(value) &= ~_BV(bit) )

volatile int count = 0; // 비교일치가 발생한 횟수 [cite: 174]
uint8_t state = 0;

ISR(TIMER0_COMPA_vect) {
    TCNT0 = 0; // 정상 모드이므로 TCNT0 수동 초기화 
    count++;
    
    // PE4는 0.5초(64회)마다 소프트웨어적으로 점멸 [cite: 544, 190]
    if (count == 64) { 
        count = 0;
        state = !state;
        if (state) set_bit(PORTE, 4);
        else clear_bit(PORTE, 4);
    }
}

int main(void) {
    set_bit(DDRB, 7); // PB7 (OCOA) 출력 설정 [cite: 312]
    set_bit(DDRE, 4); // PE4 출력 설정 [cite: 543]
    
    clear_bit(PORTB, 7);
    clear_bit(PORTE, 4);

    TCCR0B |= (1 << CS02) | (1 << CS00); // 분주비 1024 [cite: 316]
    TIMSK0 |= (1 << OCIE0A); // 비교일치 인터럽트 A 허용 [cite: 317]
    OCR0A = 128; // 비교일치 기준값 설정 [cite: 187]

    // PB7(OC0A) 핀은 하드웨어가 자동으로 반전 (빠르게 점멸) [cite: 320, 321]
    TCCR0A |= (1 << COM0A0); 

    sei(); // 전역 인터럽트 허용 [cite: 322]

    while (1) {
        // 메인 루프는 비워둠
    }
    return 0;
}