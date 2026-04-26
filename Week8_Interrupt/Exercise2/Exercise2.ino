#define F_CPU 16000000UL
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

#define set_bit(value, bit) ( _SFR_BYTE(value) |= _BV(bit) )
#define clear_bit(value, bit) ( _SFR_BYTE(value) &= ~_BV(bit) )

volatile uint8_t led_index = 0; 

ISR(INT0_vect) {
  led_index++;
  if(led_index > 3) { led_index = 0; }

  _delay_ms(200); //디바운싱로직
  EIFR = (1 << INTF0);
}

void INT0_init(void) {
  EIMSK |= (1 << INT0); 

  EICRA |= (1 << ISC01);
  EICRA &= ~(1 << ISC00);
  sei();
}

void allLEDoff(void) { 
  clear_bit(PORTE, 3);
  clear_bit(PORTE, 4);
  clear_bit(PORTE, 5);
  clear_bit(PORTG, 5);
}

int main(void) {
  INT0_init();
  clear_bit(DDRD, 0); // 버튼
  set_bit(PORTD, 0);

  set_bit(DDRE, 3);
  set_bit(DDRE, 4);
  set_bit(DDRE, 5);
  set_bit(DDRG, 5);

  uint8_t prev_index = 4;

  while (1) {
    if (led_index != prev_index) {
      allLEDoff();
      
      if(led_index == 0) { set_bit(PORTE, 4); }
      else if(led_index == 1) { set_bit(PORTE, 5); }
      else if(led_index == 2) { set_bit(PORTG, 5); }
      else if(led_index == 3) { set_bit(PORTE, 3); }

      prev_index = led_index; 
    }
  }

  return 0;
}
