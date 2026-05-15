#define F_CPU 16000000L

#include <avr/io.h>
#include <util/delay.h>
#include <stdio.h>
#include "Text_LCD.h"

void ADC_init(void) {
    ADMUX = (1 << REFS0);   // 기준 전압 AVCC 사용

    ADCSRA = (1 << ADEN)    // ADC 활성화
           | (1 << ADPS2)
           | (1 << ADPS1)
           | (1 << ADPS0);  // 분주비 128
}

int read_ADC(uint8_t channel) {
    ADMUX = (ADMUX & 0xE0) | (channel & 0x07);  // ADC 채널 선택

    ADCSRA |= (1 << ADSC);                      // ADC 변환 시작

    while (ADCSRA & (1 << ADSC));               // 변환 완료 대기

    return ADC;                                 // 10비트 ADC 값 반환
}

int main(void) {
    char buffer[20];
    int variable_value;

    LCD_init();     // 텍스트 LCD 초기화
    ADC_init();     // ADC 초기화

    LCD_clear();    // 화면 지움

    while (1) {
        variable_value = read_ADC(0);    // PF0, Arduino A0 값 읽기

        LCD_goto_XY(0, 0);               // 0행 0열로 이동
        sprintf(buffer, "%d    ", variable_value);
        LCD_write_string(buffer);        // 가변저항 값 출력

        _delay_ms(500);                  // 0.5초 대기
    }

    return 0;
}