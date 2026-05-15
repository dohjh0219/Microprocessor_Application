#define F_CPU 16000000UL
#include <avr/io.h>
#include <util/delay.h>

#define ROTATION_DELAY 1000 // 1초 대기 [cite: 471]
#define PULSE_MIN 1400      // 최소 펄스 길이 (~0도) [cite: 473]
#define PULSE_MID 3000      // 중간 펄스 길이 (~90도) [cite: 476]
#define PULSE_MAX 4600      // 최대 펄스 길이 (~180도) [cite: 478]

void timer_counter_1_init(void) {
    // 모드 14, 고속 PWM 모드 [cite: 483]
    TCCR1A = (1 << WGM11);
    TCCR1B |= (1 << WGM12) | (1 << WGM13);
    
    // 비반전 모드 (강의 노트의 덮어쓰기 오류 수정: |= 연산자 사용) [cite: 490]
    TCCR1A |= (1 << COM1A1); 
    
    // 분주율 8 [cite: 492]
    TCCR1B |= (1 << CS11); 
    
    // TOP 값 설정으로 20ms 주기 설정 [cite: 495, 497, 498]
    ICR1 = 39999; 
}

int main(void) {
    // PB5 핀을 PWM 출력으로 설정 [cite: 499]
    DDRB |= (1 << PB5); 
    timer_counter_1_init();
    
    while (1) {
        OCR1A = PULSE_MIN; // ~0도 [cite: 504]
        _delay_ms(ROTATION_DELAY);
        
        OCR1A = PULSE_MID; // ~90도 [cite: 505]
        _delay_ms(ROTATION_DELAY);
        
        OCR1A = PULSE_MAX; // ~180도 [cite: 507]
        _delay_ms(ROTATION_DELAY);
    }
    return 0;
}