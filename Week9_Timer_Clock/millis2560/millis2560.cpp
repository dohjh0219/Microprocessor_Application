#include "millis2560.h"

#define MILLIS_INCREMENT_PER_OVERFLOW (1)
#define MICROS_INCREMENT_PER_OVERFLOW (24)

volatile unsigned long timer0_millis = 0;
volatile int timer0_micros = 0;

ISR(TIMER0_OVF_vect) {
    unsigned long m = timer0_millis;
    int f = timer0_micros;

    m += MILLIS_INCREMENT_PER_OVERFLOW;
    f += MICROS_INCREMENT_PER_OVERFLOW;

    m += (f / 1000);
    f = f % 1000;

    timer0_millis = m;
    timer0_micros = f;
}

void millis2560_init(void) {
    TCCR0B |= (1 << CS01) | (1 << CS00); 
    TIMSK0 |= (1 << TOIE0); 
    sei(); 
}

unsigned long millis2560(void) {
    unsigned long m;
    uint8_t oldSREG = SREG;
    
    cli(); 
    m = timer0_millis;
    SREG = oldSREG;
    
    return m;
}