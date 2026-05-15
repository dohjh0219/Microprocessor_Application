#define F_CPU 16000000UL
#include <avr/io.h>
#include <util/delay.h>
#include "ADC.h" // 제공된 ADC 라이브러리 포함

#define set_bit(value, bit) ( _SFR_BYTE(value) |= _BV(bit))
#define clear_bit(value, bit) ( _SFR_BYTE(value) &= ~_BV(bit))

#define motor_enable1()     PORTD |= 0x80
#define motor_forward1()    PORTG |= 0x04

void port_init() {
    set_bit(DDRD, 7); // Enable
    set_bit(DDRG, 2); // Direction
    set_bit(DDRH, 6); // Speed (PH6) [cite: 517]
}

void timer_counter_2_init() {
    TCCR2A |= (1 << WGM21) | (1 << WGM20);
    TCCR2A |= (1 << COM2B1);
    TCCR2B |= (1 << CS22) | (1 << CS21) | (1 << CS20);
    OCR2B = 255; // 초기 속도 정지 (정방향 시 255가 정지)
}

int main(void) {
    port_init();
    timer_counter_2_init();
    
    // 가변저항이 연결된 채널 0번(PF0), Free Running 모드로 초기화
    ADC_init(0, FREE_RUNNING); 
    
    motor_forward1(); // 한 방향으로만 구동
    motor_enable1();
    
    while (1) {
        int adc_value = read_ADC(); // 0 ~ 1023 값 읽기
        
        // 정방향일 때 OCR2B가 0이면 최고속도, 255면 최저속도임 
        // ADC 값(0~1023)을 0~255로 스케일링 후 반전시켜 매핑
        int pwm_value = 255 - (adc_value / 4);
        
        OCR2B = pwm_value;
        _delay_ms(10);
    }
    return 0;
}