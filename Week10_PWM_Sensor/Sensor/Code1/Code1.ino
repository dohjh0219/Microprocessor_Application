#define F_CPU 16000000L
#include <avr/io.h>
#include <util/delay.h>
#include "UART0.h"
#include "ADC.h"

int main(void) {
  UART0_init();
  ADC_init(1, SINGLE_CONVERSION);

  int read, temperature;
  float input_voltage;

  while (1) {
    read = read_ADC();

    input_voltage = read * 5.0 / 1023.0;

    temperature = (int)(input_voltage * 100.0);

    // UART0_print(input_voltage);
    UART0_print(temperature);
    UART0_write('\n');

    _delay_ms(1000);
  }

  return 0;
}
