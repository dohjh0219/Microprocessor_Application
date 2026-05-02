#define F_CPU 16000000L
#include <avr/io.h>
#include <avr/interrupt.h>

#define set_bit(value, bit) ( _SFR_BYTE(value) |= _BV(bit) )
#define clear_bit(value, bit) ( _SFR_BYTE(value) &= ~_BV(bit) )

volatile int count = 0; 
uint8_t state = 0;

ISR(TIMER0_COMPA_vect) {
  count++;
  TCNT0 = 0;
}

int main(void) {
  set_bit(DDRB, 7); 
  clear_bit(PORTB, 7); 

  TCCR0B |= (1 << CS02) | (1 << CS00);

  TIMSK0 |= (1 << OCIE0A);
  OCR0A = 128;

  sei();

  while (1) {
    if (count == 64) {
      count = 0;
      state = !state;
      if (state) set_bit(PORTB, 7); 
      else clear_bit(PORTB, 7);
    }
  }

  return 0;
}
