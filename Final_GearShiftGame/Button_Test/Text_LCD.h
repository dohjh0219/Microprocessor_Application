#ifndef TEXT_LCD_H_
#define TEXT_LCD_H_

#include <avr/io.h>

// LCD 기본 함수
void LCD_init(void);
void LCD_clear(void);

// LCD 데이터/명령어 전송 함수
void LCD_write_data(uint8_t data);
void LCD_write_command(uint8_t command);

// LCD 문자열/위치 제어 함수
void LCD_write_string(char *string);
void LCD_goto_XY(uint8_t row, uint8_t col);

#endif