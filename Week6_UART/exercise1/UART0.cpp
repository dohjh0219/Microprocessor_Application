#include "UART0.h"

void UART0_init(void) {
  UBRR0H = 0x00;
  UBRR0L = 207;
  UCSR0A |= _BV(U2X0);
  UCSR0C |= 0x06;
  UCSR0B |= _BV(RXEN0);
  UCSR0B |= _BV(TXEN0);
}
void UART0_write(uint8_t data) {
  while( !(UCSR0A & (1 << UDRE0)) );
  UDR0 = data;
}
uint8_t UART0_read(void) {
  while( !(UCSR0A & (1 << RXC0)) );
  return UDR0;
}

void UART0_print(char *str) {
  while(*str)
    UART0_write(*str++);
}
void UART0_print(const char *str) {
  while(*str)
    UART0_write(*str++);
}

void UART0_print(int no, int radix) {
  char buffer[sizeof(int) * 8 + 1];
  itoa(no, buffer, radix);
  UART0_print(buffer);
}
void UART0_print(long no, int radix) {
  char buffer[sizeof(long) * 8 + 1];
  ltoa(no, buffer, radix);
  UART0_print(buffer);
}
void UART0_print(unsigned long no, int radix) {
  char buffer[sizeof(unsigned long) * 8 + 1];
  ultoa(no, buffer, radix);
  UART0_print(buffer);
}
