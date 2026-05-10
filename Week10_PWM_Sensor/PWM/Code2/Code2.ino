#define F_CPU 16000000L
#include <avr/io.h>
#include <util/delay.h>

#define set_bit(value, bit) ( _SFR_BYTE(value) |= _BV(bit) )
#define clear_bit(value, bit) ( _SFR_BYTE(value) &= ~_BV(bit) )

int main(void) {
  int dim = 0;
  int direction = 1;

  set_bit(DDRB, 7);

  TCCR0A |= (1 << WGM01) | (1 << WGM00);
  TCCR0A |= (1 << COM0A1);
  TCCR0B |= (1 << CS02) | (1 << CS00);

  while (1) {
    OCR0A = dim;
    _delay_ms(10);

    dim += direction;

    if (dim == 0) direction = 1;
    if (dim == 255) direction = -1;
  }

  return 0;
}
