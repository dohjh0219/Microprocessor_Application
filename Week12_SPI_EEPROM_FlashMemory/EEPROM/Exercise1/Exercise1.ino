#define F_CPU 16000000L
#include <avr/io.h>
#include <avr/eeprom.h>
#include <stdlib.h>
#include "UART0.h"

int main(void) {
    UART0_init();

    float writeVal = 3.14159f;
    float readVal  = 0.0f;
    int   address  = 0;

    // float 4바이트를 EEPROM 주소 0부터 블록 단위로 쓰기
    eeprom_update_block((void *)&writeVal, (void *)address, sizeof(float));

    // 같은 주소에서 float 4바이트 블록 단위로 읽기
    eeprom_read_block((void *)&readVal, (void *)address, sizeof(float));

    // float → 문자열 변환 후 시리얼 출력 (dtostrf: 전체 8자리, 소수점 5자리)
    char buf[20];
    dtostrf(readVal, 8, 5, buf);
    UART0_print("Read float: ");
    UART0_print(buf);
    UART0_write('\n');

    while (1) {}

    return 0;
}
