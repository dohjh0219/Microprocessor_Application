#define F_CPU 16000000UL

#include <avr/io.h>
#include <util/delay.h>
#include <stdlib.h>
#include "Text_LCD.h"

int main(void)
{
    LCD_init();
    LCD_clear();

    while (1)
    {
        int alphabet = rand() % 28;

        if (alphabet >= 26)
            alphabet = ' ';
        else
            alphabet += 'A';

        int x = rand() % 16;
        int y = rand() % 2; 

        LCD_goto_XY(y, x);
        LCD_write_data(alphabet);

        _delay_ms(200);
    }

    return 0;
}