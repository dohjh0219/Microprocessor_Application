#define F_CPU 16000000L
#include <avr/io.h>
#include <util/delay.h>

void UART0_init(void);
void UART0_write(uint8_t data);
uint8_t UART0_read(void);

void UART0_init(void) {
  UBRR0H = 0x00;
  UBRR0L = 207;

  UCSR0A |= _BV(U2X0);
  UCSR0C |= 0x06;

  UCSR0B |= _BV(RXEN0);
  UCSR0B |= _BV(TXEN0);
}

void UART0_write(uint8_t data) {
  while ( !(UCSR0A & (1 << UDRE0)) );
  UDR0 = data;
}
uint8_t UART0_read(void) {
  while ( !(UCSR0A & (1 << RXC0)) );
  return UDR0;
}
int main(void) {
  UART0_init();
  while (1) {
    UART0_write(UART0_read());
  }
  return 0;
}