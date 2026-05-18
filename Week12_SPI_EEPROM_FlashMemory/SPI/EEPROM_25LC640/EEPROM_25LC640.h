#ifndef _EEPROM_25LC640_
#define _EEPROM_25LC640_

#include <avr/io.h>

#define EEPROM_READ 0b00000011 	// 읽기
#define EEPROM_WRITE 0b00000010 	// 쓰기
#define EEPROM_WREN 0b00000110 	// 쓰기 허용
#define EEPROM_RDSR 0b00000101 	// 상태 레지스터 읽기

#define EEPROM_WRITE_IN_PROGRESS 0 	// 쓰기 진행 중 비트 번호

#define SPI_SS 		PL7
#define SPI_SCK 	PB1
#define SPI_MOSI 	PB2
#define SPI_MISO 	PB3

#define EEPROM_select() PORTL &= ~(1 << SPI_SS) 	// LOW
#define EEPROM_unselect() PORTL |= (1 << SPI_SS) 	// HIGH

uint8_t EEPROM_read_byte(uint16_t address);
void EEPROM_change_byte(uint8_t data);
void EEPROM_send_address(uint16_t address);
void EEPROM_write_enable(void);
void EEPROM_write_byte(uint16_t address, uint8_t data);
uint8_t EEPROM_read_status(void);
void SPI_init(void);

#endif
