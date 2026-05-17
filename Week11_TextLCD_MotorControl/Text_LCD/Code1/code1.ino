#define F_CPU 16000000L

#include <avr/io.h>
#include <util/delay.h>
#include "Text_LCD.h"

int main(void) {
    LCD_init();

    LCD_write_string("Hello World!");

    _delay_ms(1000);

    LCD_clear();

    LCD_goto_XY(0, 0);
    LCD_write_data('1');

    LCD_goto_XY(0, 5);
    LCD_write_data('2');

    LCD_goto_XY(1, 0);
    LCD_write_data('3');

    LCD_goto_XY(1, 5);
    LCD_write_data('4');

    while (1) {
 
    }

    return 0;
}