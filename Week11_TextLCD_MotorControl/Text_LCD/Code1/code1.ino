#define F_CPU 16000000L

#include <avr/io.h>
#include <util/delay.h>
#include "Text_LCD.h"

int main(void) {
    LCD_init();                         // 텍스트 LCD 초기화

    LCD_write_string("Hello World!");   // 문자열 출력

    _delay_ms(1000);                    // 1초 대기

    LCD_clear();                        // 화면 지움

    LCD_goto_XY(0, 0);                  // 0행 0열로 이동
    LCD_write_data('1');                // 문자 '1' 출력

    LCD_goto_XY(0, 5);                  // 0행 5열로 이동
    LCD_write_data('2');                // 문자 '2' 출력

    LCD_goto_XY(1, 0);                  // 1행 0열로 이동
    LCD_write_data('3');                // 문자 '3' 출력

    LCD_goto_XY(1, 5);                  // 1행 5열로 이동
    LCD_write_data('4');                // 문자 '4' 출력

    while (1) {
        // 출력 상태 유지
    }

    return 0;
}