#define F_CPU 16000000UL
#include <avr/io.h>
#include "UART0.h"

int main(void) {
    float f = 3.14159;

    UART0_init();

    UART0_print(f, 1);
    UART0_print("\n");

    UART0_print(f, 2);
    UART0_print("\n");

    UART0_print(f, 3);
    UART0_print("\n");

    UART0_print(f, 4);
    UART0_print("\n");

    UART0_print(f, 5);
    UART0_print("\n");

    while (1) {
    }

    return 0;
}
