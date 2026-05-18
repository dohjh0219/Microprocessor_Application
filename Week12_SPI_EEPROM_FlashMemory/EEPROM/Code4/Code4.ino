#include <avr/pgmspace.h>
#include "Text_LCD.h"
#define LCD_SETCGRAMADDR 0x40 // 사용자 정의 문자 설정 명령

const uint8_t user_defined[] PROGMEM = { // 사용자 정의 문자 데이터
  0B00000, 0B10001, 0B00000, 0B00000, 0B10001, 0B01110, 0B00000, 0B00000,
  0B00001, 0B00011, 0B00101, 0B01001, 0B01001, 0B01011, 0B11011, 0B11000,
  0B00000, 0B01010, 0B10101, 0B10001, 0B01010, 0B00100, 0B00000, 0B00000,
  0B01010, 0B00000, 0B00100, 0B00000, 0B01110, 0B10001, 0B10001, 0B01110
};

int main(void) {
  LCD_init(); // 텍스트 LCD 초기화
    // 사용자 정의 문자 데이터 전송
  LCD_write_command(LCD_SETCGRAMADDR);
  
  for (int i = 0; i < 32; i++) {
    LCD_write_data(pgm_read_byte(user_defined + i));
  }
  
  LCD_init();
  for (int i = 0; i < 4; i++) { // 사용자 정의 문자 출력
    LCD_write_data(i);
    LCD_write_data(' ');
  }
  
  while (1) {
  }
  
  return 0;
}