#define F_CPU 16000000L
#include <avr/io.h>
#include <avr/interrupt.h>
#include "millis2560.h"

#define set_bit(value, bit)   ( _SFR_BYTE(value) |= _BV(bit) )
#define clear_bit(value, bit) ( _SFR_BYTE(value) &= ~_BV(bit) )

int main(void) {
    set_bit(DDRB, 7); // LED 연결 핀을 출력으로 설정
    clear_bit(PORTB, 7); // LED는 꺼진 상태로 시작
    uint8_t state = 0; // LED 상태
    millis2560_init(); // 타이머/카운터 0번 초기화
    unsigned long time_previous, time_current;
    time_previous = millis2560(); // 시작 시간
    
    while (1) {
        time_current = millis2560(); // 현재 시간
        if ((time_current - time_previous) > 1000) { // 1초 경과
            time_previous = time_current;
            state = !state; // LED 상태 반전
            if (state) set_bit(PORTB, 7);
            else clear_bit(PORTB, 7);
        }
    }
    return 1;
}