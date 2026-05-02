#define F_CPU 16000000L
#include <avr/io.h>
#include <avr/interrupt.h>

#define set_bit(value, bit)   ( _SFR_BYTE(value) |= _BV(bit) )
#define clear_bit(value, bit) ( _SFR_BYTE(value) &= ~_BV(bit) )

int main(void) {
    set_bit(DDRB, 7);      // PB7 핀을 출력으로 설정
    clear_bit(PORTB, 7);   // LED는 끈 상태에서 시작

    TCCR0B |= (1 << CS02) | (1 << CS00);   // 분주비를 1024로 설정

    TIMSK0 |= (1 << OCIE0A);               // 비교일치 인터럽트 A 허용
    OCR0A = 255;                           // 비교일치 기준값

    // 비교일치 인터럽트 발생 시 OC0A 핀의 출력을 반전
    TCCR0A |= (1 << COM0A0);

    TCCR0A |= (1 << WGM01);   // CTC 모드

    sei();   // 전역적으로 인터럽트 허용

    while (1) {
    }

    return 0;
}
