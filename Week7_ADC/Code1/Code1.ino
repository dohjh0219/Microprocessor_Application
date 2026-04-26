#define F_CPU 16000000L
#include <avr/io.h>
#include <util/delay.h>
#include "UART0.h"

void ADC_init(unsigned char channel) {
  ADMUX |= (1 << REFS0);
  ADMUX = ((ADMUX & 0xE0) | (channel & 0x07));
  if (channel > 8) ADCSRB |= (1 << MUX5);

  ADCSRA |= (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0); // 128 분주비
  ADCSRA |= (1 << ADATE); // 자동 트리거 모드

  ADCSRB &= ~(1 << ADTS2) & ~(1 << ADTS1) & ~(1 << ADTS0);

  ADCSRA |= (1 << ADEN); 
  ADCSRA |= (1 << ADSC);
}

int read_ADC(void) {
  while (!(ADCSRA & (1 << ADIF)));

  return ADC;
}

int main(void) {
  int read;

  UART0_init(); 
  ADC_init(0);

  while (1) {
    read = read_ADC();

    UART0_print(read);
    UART0_write('\n');

    _delay_ms(1000);
  }

  return 0;
}
