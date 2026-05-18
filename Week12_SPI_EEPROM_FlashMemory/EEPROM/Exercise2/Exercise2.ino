#include <avr/pgmspace.h>
#include "Text_LCD.h"

#define LCD_SETCGRAMADDR 0x40

// Code4의 문자 0~3 + 추가 문자 4~7 (총 8개 × 8바이트 = 64바이트)
const uint8_t user_defined[] PROGMEM = {
    // 문자 0: 웃는 얼굴
    0B00000, 0B10001, 0B00000, 0B00000, 0B10001, 0B01110, 0B00000, 0B00000,
    // 문자 1: 사선 화살표
    0B00001, 0B00011, 0B00101, 0B01001, 0B01001, 0B01011, 0B11011, 0B11000,
    // 문자 2: 별/반짝임
    0B00000, 0B01010, 0B10101, 0B10001, 0B01010, 0B00100, 0B00000, 0B00000,
    // 문자 3: 웃는 얼굴 2
    0B01010, 0B00000, 0B00100, 0B00000, 0B01110, 0B10001, 0B10001, 0B01110,
    // 문자 4: 하트
    0B00000, 0B01010, 0B11111, 0B11111, 0B01110, 0B00100, 0B00000, 0B00000,
    // 문자 5: 위쪽 화살표
    0B00100, 0B01110, 0B11111, 0B00100, 0B00100, 0B00100, 0B00000, 0B00000,
    // 문자 6: 아래쪽 화살표
    0B00000, 0B00100, 0B00100, 0B00100, 0B11111, 0B01110, 0B00100, 0B00000,
    // 문자 7: 종(벨)
    0B00100, 0B01110, 0B01110, 0B01110, 0B11111, 0B00000, 0B00100, 0B00000
};

int main(void) {
    LCD_init();

    // CGRAM 주소 0부터 사용자 정의 문자 64바이트 전송
    LCD_write_command(LCD_SETCGRAMADDR);
    for (int i = 0; i < 64; i++) {
        LCD_write_data(pgm_read_byte(user_defined + i));
    }

    LCD_init();

    // Row 0: 문자 0~3 출력
    LCD_goto_XY(0, 0);
    for (int i = 0; i < 4; i++) {
        LCD_write_data(i);
        LCD_write_data(' ');
    }

    // Row 1: 문자 4~7 출력
    LCD_goto_XY(1, 0);
    for (int i = 4; i < 8; i++) {
        LCD_write_data(i);
        LCD_write_data(' ');
    }

    while (1) {}

    return 0;
}
