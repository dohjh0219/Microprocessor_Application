#define F_CPU 16000000L
#include <avr/io.h>
#include "UART0.h"
#include "EEPROM_25LC640.h"

union EEPROM_INT {
    int     value;      // 2바이트 int
    uint8_t bytes[2];   // 동일 메모리를 바이트 단위로 접근
};

int EEPROM_read_int(int address) {
    union EEPROM_INT data;
    data.bytes[0] = EEPROM_read_byte((uint16_t)address);
    data.bytes[1] = EEPROM_read_byte((uint16_t)(address + 1));
    return data.value;
}

void EEPROM_write_int(int address, int value) {
    union EEPROM_INT data;
    data.value = value;
    EEPROM_write_byte((uint16_t)address,       data.bytes[0]);
    EEPROM_write_byte((uint16_t)(address + 1), data.bytes[1]);
}

int main(void) {
    SPI_init();
    UART0_init();

    // 학습보드: 다른 SPI 장치 SS(PB0) HIGH로 비활성화
    DDRB  |= 0x01;
    PORTB |= 0x01;

    // int 값 쓰기
    EEPROM_write_int(0, 1234);
    EEPROM_write_int(2, -5678);
    EEPROM_write_int(4, 32767);   // int 최댓값
    EEPROM_write_int(6, -32768);  // int 최솟값

    // int 값 읽어서 출력
    UART0_print("Address 0: ");
    UART0_print(EEPROM_read_int(0));
    UART0_write('\n');

    UART0_print("Address 2: ");
    UART0_print(EEPROM_read_int(2));
    UART0_write('\n');

    UART0_print("Address 4: ");
    UART0_print(EEPROM_read_int(4));
    UART0_write('\n');

    UART0_print("Address 6: ");
    UART0_print(EEPROM_read_int(6));
    UART0_write('\n');

    while (1) {}

    return 0;
}
