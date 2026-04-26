#define F_CPU 16000000UL
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

#define set_bit(value, bit) ( _SFR_BYTE(value) |= _BV(bit) )
#define clear_bit(value, bit) ( _SFR_BYTE(value) &= ~_BV(bit) )

volatile uint8_t state = 1;

ISR(INT0_vect) {
  state = !state;
}

void INT0_init(void) {
  EIMSK |= (1 << INT0); 
  EICRA |= (1 << ISC01);
  EICRA &= ~(1 << ISC00);  // EICRA 레지스터 2개(ISC01, ISC00) 확실하게 설정하기 위함.
  sei();
}

int main(void) {
  INT0_init();
  clear_bit(DDRD, 0);
  set_bit(PORTD, 0);
  set_bit(DDRE, 3);

  while (1) {
    if (!state) set_bit(PORTE, 3);
    else clear_bit(PORTE, 3);
  }

  return 0;
}
