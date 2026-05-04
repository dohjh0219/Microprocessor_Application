#ifndef MILLIS2560_H
#define MILLIS2560_H

#include <avr/io.h>
#include <avr/interrupt.h>

void millis2560_init(void);
unsigned long millis2560(void);

#endif