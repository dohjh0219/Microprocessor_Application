#define F_CPU 16000000L
#include <avr/io.h>
#include <avr/pgmspace.h>
#include "UART0.h"

int main(void) {
  UART0_init();

  UART0_print("This is a test string.\n");

  while (1) {
  }
  
  return 0;
}