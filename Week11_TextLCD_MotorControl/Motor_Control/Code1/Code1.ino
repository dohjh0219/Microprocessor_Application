#define F_CPU 16000000UL
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

#define set_bit(value, bit) ( _SFR_BYTE(value) |= _BV(bit))
#define clear_bit(value, bit) ( _SFR_BYTE(value) &= ~_BV(bit))

// 모터 제어 허용/금지
#define motor_enable1()     PORTD |= 0x80
#define motor_disable1()    PORTD &= ~0x80
// 모터 회전 방향 설정
#define motor_forward1()    PORTG |= 0x04
#define motor_backward1()   PORTG &= ~0x04
// 모터 속도 설정
#define motor_speed1(s)     OCR2B = s

void port_init() {
    set_bit(DDRD, 7); // Enable (PD7)
    set_bit(DDRG, 2); // Direction (PG2)
    set_bit(DDRH, 6); // Speed (PH6)
    motor_disable1();
}

void timer_counter_2_init() {
    // 타이머/카운터 2번을 고속 PWM 모드로 설정
    TCCR2A |= (1 << WGM21) | (1 << WGM20);
    TCCR2A |= (1 << COM2B1); // 비반전 모드
    
    // 분주비 1024 설정 (강의 노트의 CS20 중복 오타를 CS22로 수정)
    TCCR2B |= (1 << CS22) | (1 << CS21) | (1 << CS20); 
    OCR2B = 0;
}

int main(void) {
    port_init();
    timer_counter_2_init();
    
    while (1) {
        motor_forward1();
        motor_enable1();
        motor_speed1(0); // 정방향: 0% 듀티 사이클에서 최고 속도 
        _delay_ms(3000);
        
        motor_disable1();
        _delay_ms(1000);
        
        motor_backward1();
        motor_enable1();
        motor_speed1(255); // 역방향: 100% 듀티 사이클에서 최고 속도 [cite: 291]
        _delay_ms(3000);
        
        motor_disable1();
        _delay_ms(1000);
    }
    return 0;
}