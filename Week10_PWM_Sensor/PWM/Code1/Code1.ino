#define F_CPU 16000000L
#include <avr/io.h>
#include <util/delay.h>

#define set_bit(value, bit) ( _SFR_BYTE(value) |= _BV(bit) )
#define clear_bit(value, bit) ( _SFR_BYTE(value) &= ~_BV(bit) )

#define LED_TIME 20

void turn_on_LED_in_PWM_manner(int dim) {
  int i;

  set_bit(PORTB, 7);

  for (i = 0; i < 256; i++) {
    if (i > dim) clear_bit(PORTB, 7);
    _delay_us(LED_TIME);
  }
}

int main(void) {
  set_bit(DDRB, 7);

  int dim = 0; 
  int direction = 1;

  while (1) {
    turn_on_LED_in_PWM_manner(dim);

    dim += direction;

    if (dim == 0) direction = 1;
    if (dim == 255) direction = -1;
  }

  return 0;
}
