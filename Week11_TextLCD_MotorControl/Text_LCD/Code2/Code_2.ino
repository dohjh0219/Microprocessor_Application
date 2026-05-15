#define F_CPU 16000000UL

#include <avr/io.h>
#include <util/delay.h>
#include <stdlib.h>
#include "Text_LCD.h"

int main(void)
{
    LCD_init();     // 텍스트 LCD 초기화
    LCD_clear();    // 화면 지움

    while (1)
    {
        int alphabet = rand() % 28;     // 알파벳 26글자 + 공백 2개

        if (alphabet >= 26)
            alphabet = ' ';             // 26~27은 공백으로 대체
        else
            alphabet += 'A';            // 0~25는 알파벳 대문자로 변환

        int x = rand() % 16;            // LCD 열 위치: 0~15
        int y = rand() % 2;             // LCD 행 위치: 0~1

        LCD_goto_XY(y, x);              // 임의의 위치로 커서 이동
        LCD_write_data(alphabet);       // 임의의 문자 출력

        _delay_ms(200);
    }

    return 0;
}