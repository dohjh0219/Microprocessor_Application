#define F_CPU 16000000L
#include <avr/io.h>
#include "UART0.h"
#include "EEPROM_25LC640.h"

int main(void) {
  SPI_init();
  UART0_init();

  // 학습보드의 경우, 또 다른 SPI 장치의 SS가 아두이노 53번(PB0) 핀에 연결되어
  // 있으므로, EEPROM을 선택하기 위해서는 HIGH를 가해주어야 함
  DDRB |= 0x01;
  PORTB |= 0x01;

  for (int i = 0; i < 128; i++) {     // EEPROM에 쓰기
    EEPROM_write_byte(i, i);
  }

  for (int i = 0; i < 128; i++) {     // EEPROM에서 읽기
    UART0_print("Address ");
    UART0_print(i);
    UART0_print("\t: ");
    UART0_print(EEPROM_read_byte(i));
    UART0_write('\n');
  }

  while (1) {
  }

  return 0;
}
