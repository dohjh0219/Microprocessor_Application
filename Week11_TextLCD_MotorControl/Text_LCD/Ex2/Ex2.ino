#define F_CPU 16000000UL
#include <avr/io.h>
#include <util/delay.h>
#include <stdlib.h>
#include <stdio.h>
#include "UART0.h" // 첨부된 UART0 라이브러리 포함

#define PULSE_MIN 1400 // ~0도
#define PULSE_MAX 4600 // ~180도

void timer_counter_1_init(void) {
    // 모드 14, 고속 PWM 모드
    TCCR1A = (1 << WGM11) | (1 << COM1A1);
    TCCR1B = (1 << WGM12) | (1 << WGM13) | (1 << CS11);
    ICR1 = 39999;
}

int main(void) {
    char rx_buf[10];
    int rx_idx = 0;
    
    // PB5 핀을 서보 모터 PWM 출력으로 설정
    DDRB |= (1 << PB5); 
    timer_counter_1_init();
    
    // UART 통신 초기화 (UART0.cpp의 설정 사용)
    UART0_init();
    
    while (1) {
        // UART0 라이브러리의 수신 함수 사용
        char c = UART0_read(); 
        
        // 개행 문자가 들어오면 입력 종료로 판단 ('\n' 또는 '\r')
        if (c == '\n' || c == '\r') {
            if (rx_idx > 0) {
                rx_buf[rx_idx] = '\0';
                int angle = atoi(rx_buf); // 수신된 문자열을 숫자로 변환
                
                // 각도 범위 제한 (0도 ~ 180도)
                if (angle < 0) angle = 0;
                if (angle > 180) angle = 180;
                
                // 0~180도를 1400~4600 펄스 폭으로 선형 매핑
                long pulse = PULSE_MIN + ((long)angle * (PULSE_MAX - PULSE_MIN)) / 180;
                OCR1A = (uint16_t)pulse;
                
                // 시리얼 모니터로 결과 출력
                char msg[50];
                sprintf(msg, "Angle: %d\r\nPulse Width: %ld\r\n", angle, pulse);
                UART0_print(msg); // UART0 라이브러리의 문자열 출력 함수 사용
                
                rx_idx = 0; // 버퍼 인덱스 초기화 (다음 입력을 위해)
            }
        } else {
            // 버퍼 오버플로우 방지 (최대 9글자)
            if (rx_idx < 9) {
                rx_buf[rx_idx++] = c;
            }
        }
    }
    return 0;
}