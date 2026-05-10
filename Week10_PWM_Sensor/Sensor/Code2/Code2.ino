#define F_CPU 16000000L
#include <avr/io.h>
#include <util/delay.h>
#include "UART0.h"
#include "ADC.h"

int main(void) {
  UART0_init();
  ADC_init(2);

  int read;

  while (1) {
    read = read_ADC();

    UART0_print(read);
    UART0_write('\n');

    _delay_ms(1000);
  }

  return 0;
}
