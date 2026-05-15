#define F_CPU 16000000UL
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

#define set_bit(value, bit) ( _SFR_BYTE(value) |= _BV(bit))
#define clear_bit(value, bit) ( _SFR_BYTE(value) &= ~_BV(bit))

// 모터 제어 허용/금지
#define motor_enable2()     PORTG |= 0x02
#define motor_disable2()    PORTG &= ~0x02
// 모터 회전 방향 설정
#define motor_forward2()    PORTG |= 0x01
#define motor_backward2()   PORTG &= ~0x01
// 모터 속도 설정
#define motor_speed2(s)     OCR2A = s

void port_init() {
    set_bit(DDRG, 1); // Enable (PG1) [cite: 315]
    set_bit(DDRG, 0); // Direction (PG0) [cite: 318]
    set_bit(DDRB, 4); // Speed (PB4) [cite: 332]
    motor_disable2();
}

void timer_counter_2_init() {
    // 타이머/카운터 2번을 고속 PWM 모드로 설정
    TCCR2A |= (1 << WGM21) | (1 << WGM20);
    TCCR2A |= (1 << COM2A1); // 비반전 모드
    TCCR2B |= (1 << CS22) | (1 << CS21) | (1 << CS20); // 분주비 1024
}

int main(void) {
    port_init();
    timer_counter_2_init();
    motor_enable2();
    
    while (1) {
        motor_forward2();
        // 정방향: 0% 듀티에서 최고 속도이므로 255에서 0으로 감소시켜야 속도 증가 [cite: 346, 353]
        for (int i = 255; i >= 0; i--) { 
            motor_speed2(i);
            _delay_ms(30);
        }
        _delay_ms(2000);
        
        motor_backward2();
        // 역방향: 100% 듀티에서 최고 속도이므로 0에서 255로 증가시켜야 속도 증가 [cite: 354, 363]
        for (int i = 0; i < 256; i++) { 
            motor_speed2(i);
            _delay_ms(30);
        }
        _delay_ms(2000);
    }
    return 0;
}