#define F_CPU 16000000UL
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

#define set_bit(value, bit) ( _SFR_BYTE(value) |= _BV(bit) )
#define clear_bit(value, bit) ( _SFR_BYTE(value) &= ~_BV(bit) )

volatile uint8_t state = 0; 

ISR(INT0_vect) {
  state = !state;
}

void INT0_init(void) {
  EIMSK |= (1 << INT0); 

  EICRA &= ~(1 << ISC01);
  EICRA |=  (1 << ISC00);
  sei();
}

int main(void) {
  INT0_init();
  clear_bit(DDRD, 0); // 버튼
  set_bit(PORTD, 0);

  set_bit(DDRE, 3);
  set_bit(DDRE, 4);
  set_bit(DDRE, 5);
  set_bit(DDRG, 5);

  while (1) {
    if (state) {
      set_bit(PORTE, 3);
      set_bit(PORTE, 4);
      set_bit(PORTE, 5);
      set_bit(PORTG, 5);
    } else {
      clear_bit(PORTE, 3);
      clear_bit(PORTE, 4);
      clear_bit(PORTE, 5);
      clear_bit(PORTG, 5);
    }
  }

  return 0;
}
