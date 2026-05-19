#define F_CPU 16000000L
#include <avr/io.h>
#include <util/delay.h>
#include "UART0.h"

int main(void) {
    UART0_init();                  
    UART0_print("Decimal : ");    
    UART0_print(312);             
    UART0_write('\n');  
       
    UART0_print("Binary  : 0b");
    UART0_print(312, 2);         
    UART0_write('\n');

    while (1) {
    }

    return 0;
}
