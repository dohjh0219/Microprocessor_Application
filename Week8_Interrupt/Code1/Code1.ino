#define F_CPU 16000000L
#include <avr/io.h>
#include <util/delay.h>
#include "UART0.h"

#define set_bit(value, bit) ( _SFR_BYTE(value) |= _BV(bit) )
#define clear_bit(value, bit) ( _SFR_BYTE(value) &= ~_BV(bit) )

int main(void) {
  UART0_init();
  clear_bit(DDRD, 1);
  set_bit(PORTD, 1);
  set_bit(DDRE, 3);

  while (1) {
    uint8_t state = PIND & 0x01;
    if (!state) set_bit(PORTE, 3);
    else clear_bit(PORTE, 3);

    UART0_print(!state);
    UART0_write('\n');

    _delay_ms(500);
  }

  return 0;
}