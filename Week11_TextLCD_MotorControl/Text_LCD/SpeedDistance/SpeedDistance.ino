/*
 * SpeedDistance.ino
 *
 * 동작:
 *   - UART0(9600 baud)로 속도(km/h, 정수)를 수신
 *   - LCD Row 0: 현재 속도 표시
 *   - LCD Row 1: 남은 거리(m) 실시간 계산 표시
 *
 * 시리얼 입력 형식: 정수 + 개행  (예: "80\n" → 80 km/h)
 *
 * LCD 출력 예시:
 *   Speed:  80 km/h
 *   Dist:  978.2 m
 *
 * 준비: Text_LCD.cpp / Text_LCD.h 를 이 폴더에 함께 복사
 */

#define F_CPU 16000000UL

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <stdio.h>
#include <stdlib.h>   // dtostrf
#include "Text_LCD.h"

// ── 설정 ──────────────────────────────────────────────────────────────────
#define BAUD          9600UL
#define UBRR_VAL      (F_CPU / 16 / BAUD - 1)
#define TARGET_DIST_M 1000.0f   // 목표(시작) 거리 (m)

// ── UART0 초기화 및 수신 ───────────────────────────────────────────────────
void UART_init(void) {
    UBRR0H = (uint8_t)(UBRR_VAL >> 8);
    UBRR0L = (uint8_t)(UBRR_VAL);
    UCSR0B = (1 << RXEN0) | (1 << TXEN0);
    UCSR0C = (1 << UCSZ01) | (1 << UCSZ00);  // 8비트, 패리티 없음, 정지 1비트
}

static uint8_t UART_rx_byte(void) {
    while (!(UCSR0A & (1 << RXC0)));
    return UDR0;
}

// "숫자\n" 또는 "숫자\r\n" 을 수신하여 int16_t 로 파싱
int16_t UART_receive_speed(void) {
    char    buf[8];
    uint8_t i = 0;
    char    c;

    while (1) {
        c = (char)UART_rx_byte();
        if (c == '\n' || c == '\r') {
            if (i > 0) break;   // 유효 문자가 있을 때만 종료
            continue;           // 선행 \r 무시
        }
        if (i < (uint8_t)(sizeof(buf) - 1)) buf[i++] = c;
    }
    buf[i] = '\0';

    int16_t val   = 0;
    uint8_t start = (buf[0] == '-') ? 1 : 0;
    for (uint8_t j = start; j < i; j++) {
        if (buf[j] >= '0' && buf[j] <= '9')
            val = val * 10 + (buf[j] - '0');
    }
    return (buf[0] == '-') ? -val : val;
}

// ── Timer1: 경과 시간 측정 ─────────────────────────────────────────────────
// Normal mode, prescaler 1024 → 1 tick = 64 µs
// 오버플로(65536 ticks) = 약 4194 ms
volatile uint32_t g_ovf = 0;

ISR(TIMER1_OVF_vect) { g_ovf++; }

void timer1_init(void) {
    TCCR1A = 0x00;
    TCCR1B = (1 << CS12) | (1 << CS10);   // prescaler 1024, normal mode
    TIMSK1 = (1 << TOIE1);                 // overflow interrupt 허용
    sei();
}

// 마지막 리셋 이후 경과 시간 반환 (ms)
static uint32_t elapsed_ms(void) {
    cli();
    uint32_t ovf = g_ovf;
    uint16_t cnt = TCNT1;
    sei();
    return (ovf * 65536UL + (uint32_t)cnt) * 64UL / 1000UL;
}

static void timer1_reset(void) {
    cli();
    g_ovf = 0;
    TCNT1 = 0;
    sei();
}

// ── LCD 화면 갱신 ──────────────────────────────────────────────────────────
// Row 0 (16자): "Speed: XXX km/h "
// Row 1 (16자): "Dist: XXXXXXX m "  (소수 1자리)
static void LCD_update(int16_t speed_kmh, float remaining_m) {
    char line[17];
    char dist_str[10];

    if (remaining_m < 0.0f) remaining_m = 0.0f;

    // Row 0
    snprintf(line, sizeof(line), "Speed: %3d km/h ", speed_kmh);
    LCD_goto_XY(0, 0);
    LCD_write_string(line);

    // Row 1 (dtostrf: 최소 7자리, 소수 1자리 → "1000.0" ~ "   0.0")
    dtostrf(remaining_m, 7, 1, dist_str);
    snprintf(line, sizeof(line), "Dist:%s m  ", dist_str);  // 5+7+4 = 16자
    LCD_goto_XY(1, 0);
    LCD_write_string(line);
}

// ── main ──────────────────────────────────────────────────────────────────
int main(void) {
    LCD_init();
    UART_init();
    timer1_init();

    float   remaining_m = TARGET_DIST_M;
    int16_t speed_kmh   = 0;
    uint8_t first       = 1;   // 첫 수신: 타이머 기준점만 설정, 거리 갱신 없음

    LCD_clear();
    LCD_update(0, remaining_m);

    while (1) {
        // 시리얼에서 속도 수신 (블로킹)
        speed_kmh = UART_receive_speed();

        if (first) {
            // 첫 수신: 기준 시점 설정만, 거리 계산 건너뜀
            first = 0;
        } else {
            // 이전 수신 이후 경과 시간으로 이동 거리 계산
            uint32_t dt_ms = elapsed_ms();
            float speed_ms = (float)speed_kmh / 3.6f;          // km/h → m/s
            float moved_m  = speed_ms * ((float)dt_ms / 1000.0f);
            remaining_m   -= moved_m;
            if (remaining_m < 0.0f) remaining_m = 0.0f;
        }

        timer1_reset();
        LCD_update(speed_kmh, remaining_m);
    }

    return 0;
}
