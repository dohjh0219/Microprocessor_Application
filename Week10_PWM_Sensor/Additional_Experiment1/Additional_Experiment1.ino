#define F_CPU 16000000L

#include <avr/io.h>
#include <util/delay.h>
#include "UART0.h"
#include "ADC.h"

#define set_bit(value, bit)   (_SFR_BYTE(value) |= _BV(bit))
#define clear_bit(value, bit) (_SFR_BYTE(value) &= ~_BV(bit))

/*
    추가 실습: 조도 센서 기반 LED 1개 PWM 밝기 제어

    조도 센서: ADC2, Arduino A2
    LED 출력: PB7, Arduino 13번, OC0A

    동작:
    밝음   → ADC 값 작음 → PWM 듀티비 작음 → LED 어두움
    어두움 → ADC 값 큼   → PWM 듀티비 큼   → LED 밝음
*/

void PWM_init(void) {
    // PB7, Arduino 13번 핀을 출력으로 설정
    set_bit(DDRB, 7);

    // Timer/Counter0을 Fast PWM 모드로 설정
    // WGM01 = 1, WGM00 = 1 → Fast PWM, TOP = 0xFF
    TCCR0A |= (1 << WGM01) | (1 << WGM00);

    // OC0A 비반전 모드 설정
    // 비교일치 발생 시 OC0A LOW, BOTTOM에서 HIGH
    TCCR0A |= (1 << COM0A1);

    // 분주비 64 설정
    // PWM 주파수 = 16MHz / (64 * 256) ≈ 976Hz
    TCCR0B |= (1 << CS01) | (1 << CS00);

    // 초기 LED OFF
    OCR0A = 0;
}

int main(void) {
    int light_value;
    int duty;

    UART0_init();

    // 조도 센서 ADC 채널 2번 초기화
    ADC_init(2, SINGLE_CONVERSION);

    // PWM 초기화
    PWM_init();

    while (1) {
        // 조도 센서값 읽기
        light_value = read_ADC();   // 0~1023

        // ADC 값 0~1023을 PWM 듀티비 0~255로 변환
        duty = light_value / 4;

        // PWM 듀티비 적용
        OCR0A = duty;

        // 시리얼 모니터 출력
        UART0_print(light_value);
        UART0_write('\t');
        UART0_print(duty);
        UART0_write('\n');

        _delay_ms(100);
    }

    return 0;
}