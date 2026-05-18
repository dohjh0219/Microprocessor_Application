#include "EEPROM_25LC640.h"

uint8_t EEPROM_read_byte(uint16_t address) {
	EEPROM_select(); 			// EEPROM 선택

	EEPROM_change_byte(EEPROM_READ);	// 읽기 명령 전송
	EEPROM_send_address(address); 	// 메모리 주소 전송
	// 마스터에서 바이트 값을 전송하여야 슬레이브로부터 바이트 값을 받을 수 있으므로
	// 의미 없는 0을 전송
	EEPROM_change_byte(0);

	EEPROM_unselect(); 			// EEPROM 선택 해제

	return SPDR;
}

void EEPROM_change_byte(uint8_t data) {
	SPDR = data; 				// 데이터 전송 시작
	loop_until_bit_is_set(SPSR, SPIF); 	// 전송 완료 대기
}

void EEPROM_send_address(uint16_t address) {
	// 상위 바이트를 먼저 전송하고 하위 바이트를 전송
	EEPROM_change_byte((uint8_t)(address >> 8));
	EEPROM_change_byte((uint8_t)address);
}

void EEPROM_write_enable(void) {
	EEPROM_select(); 			// Slave Select를 LOW로
	EEPROM_change_byte(EEPROM_WREN);	// 쓰기 가능하도록 설정
	EEPROM_unselect(); 			// Slave Select를 HIGH로
}

void EEPROM_write_byte(uint16_t address, uint8_t data) {
	EEPROM_write_enable(); 		// 쓰기 가능 모드로 설정

	EEPROM_select(); 			// EEPROM 선택
	EEPROM_change_byte(EEPROM_WRITE); 	// 쓰기 명령 전송
	EEPROM_send_address(address); 	// 주소 전송
	EEPROM_change_byte(data); 		// 데이터 전송

	EEPROM_unselect(); 			// EEPROM 선택 해제

	// 쓰기가 완료될 때까지 대기
	while (EEPROM_read_status() & _BV(EEPROM_WRITE_IN_PROGRESS));
}

uint8_t EEPROM_read_status(void) {
	EEPROM_select(); 			// EEPROM 선택
	EEPROM_change_byte(EEPROM_RDSR); 	// 상태 레지스터 읽기 명령 전송
	EEPROM_change_byte(0); 		// 상태 레지스터 값 읽기
	EEPROM_unselect(); 			// EEPROM 선택 해제

	return SPDR;
}

void SPI_init(void) {
	DDRL |= (1 << SPI_SS); 		// SS 핀을 출력으로 설정
	// SS핀은 HIGH로 설정하여 EEPROM이 선택되지 않은 상태로 시작
	PORTL |= (1 << SPI_SS);

	DDRB |= (1 << SPI_MOSI); 		// MOSI 핀을 출력으로 설정
	DDRB &= ~(1 << SPI_MISO); 		// MISO 핀을 입력으로 설정
	DDRB |= (1 << SPI_SCK); 		// SCK 핀을 출력으로 설정

	SPCR |= (1 << MSTR); 			// 마스터 모드
	SPCR |= (1 << SPE); 			// SPI 활성화
}
