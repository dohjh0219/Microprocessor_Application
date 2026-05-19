#define F_CPU 16000000L
#include <avr/io.h>
#include "UART0.h"

void init_LEDs(void) {
    DDRE |= (1 << PE3) | (1 << PE4) | (1 << PE5);
    DDRG |= (1 << PG5);
    
    PORTE &= ~((1 << PE3) | (1 << PE4) | (1 << PE5)); // initial condition
    PORTG &= ~(1 << PG5);
}

void turn_off_all_LEDs(void) {
    PORTE &= ~((1 << PE3) | (1 << PE4) | (1 << PE5));
    PORTG &= ~(1 << PG5);
}

int main(void) {
    UART0_init();
    init_LEDs();

    char data;

    while (1) {
        data = UART0_read(); 

        if (data == '\n' || data == '\r') {
            continue; 
        }

        turn_off_all_LEDs(); 

        switch (data) {
            case '2':
                PORTE |= (1 << PE4); 
                UART0_print("LED on Pin 2 is ON.\n");
                break;
            case '3': 
                PORTE |= (1 << PE5); 
                UART0_print("LED on Pin 3 is ON.\n");
                break;
            case '4':
                PORTG |= (1 << PG5); 
                UART0_print("LED on Pin 4 is ON.\n");
                break;
            case '5':
                PORTE |= (1 << PE3); 
                UART0_print("LED on Pin 5 is ON.\n");
                break;
            default:
                UART0_print("All LEDs are OFF\n");
                break;
        }
    }

    return 0;
}
