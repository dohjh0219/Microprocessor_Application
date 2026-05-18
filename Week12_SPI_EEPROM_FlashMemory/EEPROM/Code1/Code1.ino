#include <avr/eeprom.h>
#include "UART0.h"

int main(void) {
  // EEPROM에 쓸 데이터
  uint8_t dataByte = 127, restoreByte = 0;
  int dataWord = 32767, restoreWord = 0;
  char dataString[] = "ABCDE", restoreString[6] = "";
  
  // EEPROM의 주소
  int addressByte = 0, addressWord = 10, addressString = 20;
  
  UART0_init();
  
  // 데이터 쓰기
  eeprom_update_byte ( (uint8_t *)addressByte, dataByte);
  eeprom_update_word ( (uint16_t *)addressWord, dataWord);
  eeprom_update_block ( (void *)dataString, (void *)addressString, 5);
  
  // 데이터 읽기
  restoreByte = eeprom_read_byte ( (uint8_t *)addressByte );
  restoreWord = eeprom_read_word ( (uint16_t *)addressWord );
  eeprom_read_block ( (void *)restoreString, (void *)addressString, 5);
  
  UART0_print(restoreByte); UART0_write('\n');
  UART0_print(restoreWord); UART0_write('\n');
  UART0_print(restoreString); UART0_write('\n');
  
  while (1) {
  }

  return 0;
}