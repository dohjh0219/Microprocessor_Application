/*
 * ================================================================
 *  버튼 테스트 (검증된 핀만 사용)
 *
 *  기존 수업 코드 기반 (code_3, F1_Game, Week8 Code2 방식 동일)
 *
 *  핀 배치
 *  PJ1 (D14) : 풀다운 → 누르면 HIGH  → 1단/SHIFT-UP
 *  PJ0 (D15) : 풀다운 → 누르면 HIGH  → CLUTCH
 *  PH1 (D16) : 풀업   → 누르면 LOW   → 2단/SHIFT-DOWN
 *  PD0 (D21) : 내부풀업 → 누르면 LOW → ACCEL (외부 버튼)
 *
 *  LCD 1행: CL  UP  DN  AC
 *  LCD 2행: 0   0   0   0   (1=눌림)
 * ================================================================
 */

#define F_CPU 16000000UL
#include <avr/io.h>
#include <util/delay.h>
#include "Text_LCD.h"

#define set_bit(r,b)   (_SFR_BYTE(r) |=  _BV(b))
#define clear_bit(r,b) (_SFR_BYTE(r) &= ~_BV(b))

/* ---- UART ---- */
#define UBRR_VAL (F_CPU / 16 / 9600UL - 1)
static void uart_init(void) {
    UBRR0H = (uint8_t)(UBRR_VAL >> 8);
    UBRR0L = (uint8_t) UBRR_VAL;
    UCSR0B = (1 << TXEN0);
    UCSR0C = (1 << UCSZ01) | (1 << UCSZ00);
}
static void uart_putc(char c) {
    while (!(UCSR0A & (1 << UDRE0)));
    UDR0 = (uint8_t)c;
}
static void uart_puts(const char *s) { while (*s) uart_putc(*s++); }

/* ---- LCD ---- */
static void LCD_puts(const char *s) {
    while (*s) LCD_write_data((uint8_t)*s++);
}

/* ================================================================
 *  포트 초기화 (기존 수업 코드와 완전히 동일한 방식)
 * ================================================================ */
static void ports_init(void) {
    /* PJ0, PJ1: 풀다운 버튼 → 입력, 내부풀업 OFF (code_3 / F1_Game 동일) */
    DDRJ &= ~0x03;

    /* PH1: 풀업 버튼 → 입력, 내부풀업 OFF (code_3 동일) */
    DDRH &= ~0x02;

    /* PD0: 내부풀업 버튼 → 입력, 내부풀업 ON (Week8 Code2 동일) */
    clear_bit(DDRD, 0);
    set_bit(PORTD, 0);
}

/* ================================================================
 *  버튼 읽기 (기존 수업 코드와 동일한 읽기 방식)
 *
 *  CL  (CLUTCH)   : PJ0 풀다운  → 누르면 HIGH  → (PINJ & 0x01) == 1
 *  UP  (SHIFT-UP) : PJ1 풀다운  → 누르면 HIGH  → (PINJ >> 1) & 0x01 == 1
 *  DN  (SHIFT-DN) : PH1 풀업    → 누르면 LOW   → (PINH >> 1) & 0x01 == 0
 *  AC  (ACCEL)    : PD0 내부풀업 → 누르면 LOW  → (PIND & 0x01) == 0
 * ================================================================ */
static uint8_t btn_CL(void) { return  (PINJ & 0x01);             }
static uint8_t btn_UP(void) { return  (PINJ >> 1) & 0x01;        }
static uint8_t btn_DN(void) { return !((PINH >> 1) & 0x01);      }
static uint8_t btn_AC(void) { return !(PIND & 0x01);             }

int main(void) {
    ports_init();
    LCD_init();
    uart_init();

    LCD_clear();
    LCD_goto_XY(0, 0);  LCD_puts("CL  UP  DN  AC  ");
    LCD_goto_XY(1, 0);  LCD_puts("0   0   0   0   ");

    uart_puts("\r\n=== BUTTON TEST ===\r\n");
    uart_puts("CL=PJ0 UP=PJ1 DN=PH1 AC=PD0\r\n\n");

    uint16_t tick = 0;

    while (1) {
        uint8_t cl = btn_CL();
        uint8_t up = btn_UP();
        uint8_t dn = btn_DN();
        uint8_t ac = btn_AC();

        /* LCD 2행 업데이트 */
        LCD_goto_XY(1, 0);
        LCD_write_data(cl ? '1' : '0');  LCD_write_data(' ');
        LCD_write_data(' ');  LCD_write_data(' ');
        LCD_write_data(up ? '1' : '0');  LCD_write_data(' ');
        LCD_write_data(' ');  LCD_write_data(' ');
        LCD_write_data(dn ? '1' : '0');  LCD_write_data(' ');
        LCD_write_data(' ');  LCD_write_data(' ');
        LCD_write_data(ac ? '1' : '0');

        /* UART 500ms마다 */
        if (tick % 10 == 0) {
            uart_puts("CL="); uart_putc('0' + cl);
            uart_puts(" UP="); uart_putc('0' + up);
            uart_puts(" DN="); uart_putc('0' + dn);
            uart_puts(" AC="); uart_putc('0' + ac);
            uart_puts("\r\n");
        }
        tick++;
        _delay_ms(50);
    }
    return 0;
}
