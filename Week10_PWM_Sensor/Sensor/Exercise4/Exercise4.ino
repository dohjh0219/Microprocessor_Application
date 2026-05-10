#define F_CPU 16000000L
#include <avr/io.h>
#include <util/delay.h>
#include "UART0.h"
#include "ADC.h"

#define set_bit(value, bit) ( _SFR_BYTE(value) |= _BV(bit) )
#define clear_bit(value, bit) ( _SFR_BYTE(value) &= ~_BV(bit) )

#define THRESHOLD 30

int main(void) {
  UART0_init();
  ADC_init(2, SINGLE_CONVERSION);

  set_bit(DDRB, 7);
  clear_bit(PORTB, 7);

  int read;

  while (1) {
    read = read_ADC();

    UART0_print(read);

    if (read >= THRESHOLD) {
      UART0_print("\t LED is on.\n");
      set_bit(PORTB, 7);
    }
    else {
      UART0_print("\t LED is OFF.\n");
      clear_bit(PORTB, 7);
    }

    _delay_ms(1000);
  }

  return 0;
}
