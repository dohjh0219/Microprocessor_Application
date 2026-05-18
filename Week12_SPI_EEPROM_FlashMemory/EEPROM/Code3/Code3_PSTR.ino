#define F_CPU 16000000L
#include <avr/io.h>
#include <avr/pgmspace.h>
#include "UART0.h"

void UART0_print_flash(const char *str) {
  int count = 0;
  while (pgm_read_byte(str + count))
    UART0_write(pgm_read_byte(str + count++));
}
int main(void) {
  UART0_init();
  
  UART0_print_flash(PSTR("This is a test string.\n"));
  UART0_print_flash(PSTR("This is a test string.\n"));
  UART0_print_flash(PSTR("This is a test string.\n"));
  UART0_print_flash(PSTR("This is a test string.\n"));
  while (1) {
  }

  return 0;
}