#define F_CPU 16000000UL

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include "Text_LCD.h"

#define BAUD     9600UL
#define UBRR_VAL (F_CPU / 16 / BAUD - 1)

// 시작 거리: 1000m → cm 단위로 관리 (소수 1자리 표현 가능)
#define TARGET_DIST_CM 100000L

// ── UART0 ─────────────────────────────────────────────────────────────────
void UART_init(void) {
    UBRR0H = (uint8_t)(UBRR_VAL >> 8);
    UBRR0L = (uint8_t)(UBRR_VAL);
    UCSR0B = (1 << RXEN0) | (1 << TXEN0);
    UCSR0C = (1 << UCSZ01) | (1 << UCSZ00);
}

static uint8_t UART_rx(void) {
    while (!(UCSR0A & (1 << RXC0)));
    return UDR0;
}

// "정수\n" 수신 → int16_t 반환 (예: "80\n" → 80)
int16_t UART_receive_speed(void) {
    char    buf[8];
    uint8_t i = 0;
    char    c;

    while (1) {
        c = (char)UART_rx();
        if (c == '\n' || c == '\r') {
            if (i > 0) break;
            continue;   // 선행 \r 무시
        }
        if (i < (uint8_t)(sizeof(buf) - 1)) buf[i++] = c;
    }
    buf[i] = '\0';

    int16_t val = 0;
    for (uint8_t j = 0; j < i; j++) {
        if (buf[j] >= '0' && buf[j] <= '9')
            val = val * 10 + (buf[j] - '0');
    }
    return val;
}

// ── Timer1: 경과 시간 측정 ─────────────────────────────────────────────────
// prescaler 1024 → 1 tick = 64µs, overflow(65536 ticks) ≈ 4194ms
volatile uint32_t g_ovf = 0;

ISR(TIMER1_OVF_vect) { g_ovf++; }

void timer1_init(void) {
    TCCR1A = 0x00;
    TCCR1B = (1 << CS12) | (1 << CS10);  // prescaler 1024, normal mode
    TIMSK1 = (1 << TOIE1);
    sei();
}

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

// ── LCD 출력 헬퍼 ─────────────────────────────────────────────────────────

// const char* 리터럴을 LCD에 출력 (LCD_write_string은 char*라 래퍼 사용)
static void LCD_puts(const char *s) {
    while (*s) LCD_write_data((uint8_t)*s++);
}

// val을 width 자리 우측 정렬로 출력 (빈 자리는 공백)
static void LCD_write_padded(uint32_t val, uint8_t width) {
    char    buf[6];
    uint8_t len = 0;

    if (val == 0) {
        buf[len++] = '0';
    } else {
        while (val > 0 && len < (uint8_t)sizeof(buf)) {
            buf[len++] = '0' + (uint8_t)(val % 10);
            val /= 10;
        }
    }
    // 남은 자리 공백 패딩
    for (uint8_t p = len; p < width; p++) LCD_write_data(' ');
    // 역순 출력
    while (len > 0) LCD_write_data((uint8_t)buf[--len]);
}

// ── LCD 화면 갱신 ─────────────────────────────────────────────────────────
// Row 0 (16자): "Speed: XXX km/h "
// Row 1 (16자): "Dist: XXXX.X m  "
static void LCD_update(int16_t speed_kmh, int32_t remaining_cm) {
    if (remaining_cm < 0) remaining_cm = 0;

    uint32_t m_int = (uint32_t)(remaining_cm / 100);       // 정수 미터
    uint8_t  m_dec = (uint8_t)((remaining_cm % 100) / 10); // 소수 첫째 자리

    // Row 0
    LCD_goto_XY(0, 0);
    LCD_puts("Speed: ");                       // 7자
    LCD_write_padded((uint32_t)speed_kmh, 3);  // 3자
    LCD_puts(" km/h ");                        // 6자  합계 16

    // Row 1
    LCD_goto_XY(1, 0);
    LCD_puts("Dist: ");                        // 6자
    LCD_write_padded(m_int, 4);                // 4자
    LCD_write_data('.');
    LCD_write_data('0' + m_dec);               // 소수 1자리
    LCD_puts(" m  ");                          // 4자  합계 16
}

// ── main ──────────────────────────────────────────────────────────────────
int main(void) {
    LCD_init();
    UART_init();
    timer1_init();

    int32_t remaining_cm = TARGET_DIST_CM;
    int16_t speed_kmh    = 0;
    uint8_t first        = 1;   // 첫 수신: 타이머 기준점만 설정

    LCD_clear();
    LCD_update(0, remaining_cm);

    while (1) {
        speed_kmh = UART_receive_speed();

        if (first) {
            first = 0;
        } else {
            uint32_t dt_ms = elapsed_ms();

            // 너무 긴 대기 시간은 60초로 제한 (정수 오버플로 방지)
            if (dt_ms > 60000UL) dt_ms = 60000UL;

            // 이동 거리(cm) = speed(km/h) × dt(ms) / 36
            // 유도: speed(m/s) = speed/3.6, dist(m) = speed/3.6 × dt/1000
            //       dist(cm) = dist(m) × 100 = speed × dt / 36
            int32_t moved_cm = (int32_t)speed_kmh * (int32_t)dt_ms / 36L;
            remaining_cm    -= moved_cm;
            if (remaining_cm < 0) remaining_cm = 0;
        }

        timer1_reset();
        LCD_update(speed_kmh, remaining_cm);
    }

    return 0;
}
