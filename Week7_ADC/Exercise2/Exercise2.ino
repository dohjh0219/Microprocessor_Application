#define F_CPU 16000000L
#include <avr/io.h>
#include <util/delay.h>
#include "UART0.h"
#include "ADC.h"

int main(void) {
  UART0_init(); // UART 통신 초기화
  ADC_init(0, FREE_RUNNING); // AD 변환기 초기화

  while (1) {
    int read = read_ADC(); // 가변저항 읽기
    
    UART0_print(read);
    UART0_write('\n');

    _delay_ms(1000); // 1초에 한 번 읽음
  }
  return 0;
}