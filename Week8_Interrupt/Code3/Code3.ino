#define F_CPU 16000000UL
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

#define set_bit(value, bit) ( _SFR_BYTE(value) |= _BV(bit) )
#define clear_bit(value, bit) ( _SFR_BYTE(value) &= ~_BV(bit) )

volatile uint8_t state = 0; 

ISR(PCINT1_vect) {
  state = !state;
}

void PCINT1_init(void) {
  PCICR |= (1 << PCIE1); 
  PCMSK1 |= (1 << PCINT9);
  sei();
}

int main(void) {
  PCINT1_init(); 
  clear_bit(DDRJ, 0);
  set_bit(DDRE, 5);

  while (1) {
    if (state) set_bit(PORTE, 5);
    else clear_bit(PORTE, 5);
  }

  return 0;
}
