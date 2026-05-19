#ifndef _UART0_LIBRARY_
#define _UART0_LIBRARY_
#include <avr/io.h>
#include <stdlib.h>


void UART0_init();
uint8_t UART0_read();
void UART0_write(uint8_t data);
void UART0_print(char *str);
void UART0_print(const char *str);
void UART0_print(int no, int radix = 10);
void UART0_print(long no, int radix = 10);
void UART0_print(unsigned long no, int radix = 10);

#endif