#define F_CPU 16000000L
#include <avr/io.h>
#include <util/delay.h>
#include "UART0.h"
#include "ADC.h"

#define set_bit(value, bit) ( _SFR_BYTE(value) |= _BV(bit) )
#define clear_bit(value, bit) ( _SFR_BYTE(value) &= ~_BV(bit) )

#define THRESHOLD 28

volatile int count = 0;
volatile uint8_t state = 0;

ISR(TIMER0_OVF_vect) {
  count++;
  if (count == 32) {
    count = 0;
    state = !state;
    if (state) set_bit(PORTB, 7);
    else clear_bit(PORTB, 7);
  }
}

int main(void) {
  UART0_init();
  ADC_init(1);

  TCCR0B |= (1 << CS02) | (1 << CS00);
  sei();

  set_bit(DDRB, 7);
  clear_bit(PORTB, 7);

  int read, temperature;

  while (1) {
    read = read_ADC();
    temperature = (int)(read * 500.0 / 1023.0);

    if (temperature < THRESHOLD) {
      TIMSK0 &= ~(1 << TOIE0);
      clear_bit(PORTB, 7);
    }
    else {
      TIMSK0 |= (1 << TOIE0);
    }

    UART0_print(temperature);
    UART0_write('\n');

    _delay_ms(1000);
  }

  return 0;
}
