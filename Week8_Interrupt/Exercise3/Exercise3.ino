#define F_CPU 16000000UL
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

#define set_bit(value, bit) ( _SFR_BYTE(value) |= _BV(bit) )
#define clear_bit(value, bit) ( _SFR_BYTE(value) &= ~_BV(bit) )

volatile uint8_t led_state = 0; 

ISR(PCINT1_vect) {
  if ((PINJ & (1 << PJ0)) || (PINJ & (1 << PJ1))) {
    _delay_ms(200);
    EIFR = (1 << INTF0);

    led_state = !led_state; // LED 상태 반전
  }
}

void PCINT1_init(void) {
  PCICR |= (1 << PCIE1); 
  PCMSK1 |= (1 << PCINT9) | (1 << PCINT10);
  sei();
}

int main(void) {
  PCINT1_init(); 

  clear_bit(DDRJ, 0);
  clear_bit(DDRJ, 1);
  set_bit(DDRE, 4);
  set_bit(DDRE, 5);

  while (1) {
    if (led_state) { set_bit(PORTE, 5); set_bit(PORTE, 4); }
    else { clear_bit(PORTE, 5); clear_bit(PORTE,4); }
  }

  return 0;
}