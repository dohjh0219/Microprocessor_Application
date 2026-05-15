#define F_CPU 16000000L

#include <avr/io.h>
#include <util/delay.h>
#include "Text_LCD.h"

#define set_bit(value, bit)   (_SFR_BYTE(value) |= _BV(bit))
#define clear_bit(value, bit) (_SFR_BYTE(value) &= ~_BV(bit))

// LCD 제어 핀
#define RS_PIN 5
#define E_PIN  4

// LCD 명령어
#define COMMAND_CLEAR_DISPLAY 0x01
#define COMMAND_4_BIT_MODE    0x28    // 4비트, 2라인, 5x8 폰트

#define COMMAND_DISPLAY_ON_OFF_BIT 2
#define COMMAND_CURSOR_ON_OFF_BIT  1
#define COMMAND_BLINK_ON_OFF_BIT   0


// 비트 순서 뒤집기 함수
static uint8_t reverse_bit_order(uint8_t data) {
    uint8_t result = 0;

    if (data & (1 << 0)) result |= (0x80 >> 0);
    if (data & (1 << 1)) result |= (0x80 >> 1);
    if (data & (1 << 2)) result |= (0x80 >> 2);
    if (data & (1 << 3)) result |= (0x80 >> 3);
    if (data & (1 << 4)) result |= (0x80 >> 4);
    if (data & (1 << 5)) result |= (0x80 >> 5);
    if (data & (1 << 6)) result |= (0x80 >> 6);
    if (data & (1 << 7)) result |= (0x80 >> 7);

    return result;
}


// LCD Enable 펄스 발생 함수
static void LCD_pulse_enable(void) {
    set_bit(PORTL, E_PIN);      // E = HIGH
    _delay_us(1);

    clear_bit(PORTL, E_PIN);    // E = LOW, 하강 에지에서 동작
    _delay_ms(1);
}


// LCD에 문자 데이터 전송
void LCD_write_data(uint8_t data) {
    set_bit(PORTL, RS_PIN);     // RS = 1, 데이터 레지스터 선택

    uint8_t temp = reverse_bit_order(data);

    // 상위 4비트 전송
    PORTL = (PORTL & 0xF0) | (temp & 0x0F);
    LCD_pulse_enable();

    // 하위 4비트 전송
    PORTL = (PORTL & 0xF0) | (temp >> 4);
    LCD_pulse_enable();

    _delay_ms(2);
}


// LCD에 명령어 전송
void LCD_write_command(uint8_t command) {
    clear_bit(PORTL, RS_PIN);   // RS = 0, 명령어 레지스터 선택

    uint8_t temp = reverse_bit_order(command);

    // 상위 4비트 전송
    PORTL = (PORTL & 0xF0) | (temp & 0x0F);
    LCD_pulse_enable();

    // 하위 4비트 전송
    PORTL = (PORTL & 0xF0) | (temp >> 4);
    LCD_pulse_enable();

    _delay_ms(2);
}


// LCD 화면 지우기
void LCD_clear(void) {
    LCD_write_command(COMMAND_CLEAR_DISPLAY);
    _delay_ms(2);
}


// LCD 초기화
void LCD_init(void) {
    _delay_ms(50);      // LCD 초기 구동 대기

    DDRL |= 0x0F;       // PL0~PL3 데이터 핀 출력 설정
    DDRL |= (1 << RS_PIN) | (1 << E_PIN);   // RS, E 출력 설정

    LCD_write_command(0x02);               // 4비트 모드 진입
    LCD_write_command(COMMAND_4_BIT_MODE); // 4비트, 2라인, 5x8 폰트 설정

    // Display ON, Cursor OFF, Blink OFF
    uint8_t command = 0x08 | (1 << COMMAND_DISPLAY_ON_OFF_BIT);
    LCD_write_command(command);

    LCD_clear();

    // Entry Mode Set
    // 문자 출력 후 커서를 오른쪽으로 이동
    LCD_write_command(0x06);
}


// 문자열 출력
void LCD_write_string(char *string) {
    uint8_t i;

    for (i = 0; string[i]; i++) {
        LCD_write_data(string[i]);
    }
}


// LCD 출력 위치 설정
void LCD_goto_XY(uint8_t row, uint8_t col) {
    col %= 16;      // 열 범위: 0~15
    row %= 2;       // 행 범위: 0~1

    // 0행 시작 주소: 0x00
    // 1행 시작 주소: 0x40
    uint8_t address = (0x40 * row) + col;
    uint8_t command = 0x80 + address;

    LCD_write_command(command);
}