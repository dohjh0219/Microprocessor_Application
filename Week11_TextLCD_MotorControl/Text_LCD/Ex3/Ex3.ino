#define F_CPU 16000000UL
#include <avr/io.h>
#include <util/delay.h>
#include "ADC.h"

#define PULSE_MIN 1400 // ~0도 [cite: 473]
#define PULSE_MAX 4600 // ~180도 [cite: 478]

void timer_counter_1_init(void) {
    TCCR1A = (1 << WGM11) | (1 << COM1A1);
    TCCR1B = (1 << WGM12) | (1 << WGM13) | (1 << CS11);
    ICR1 = 39999;
}

int main(void) {
    DDRB |= (1 << PB5); 
    timer_counter_1_init();
    
    // PF0 (A0) 채널을 Free Running 모드로 초기화
    ADC_init(0, FREE_RUNNING);
    
    while (1) {
        int adc_value = read_ADC(); // 가변저항 값(0~1023) 읽기 
        
        // ADC 값(0~1023)을 펄스 폭(1400~4600)으로 선형 매핑
        long pulse = PULSE_MIN + ((long)adc_value * (PULSE_MAX - PULSE_MIN)) / 1023;
        OCR1A = (uint16_t)pulse;
        
        _delay_ms(20);
    }
    return 0;
}