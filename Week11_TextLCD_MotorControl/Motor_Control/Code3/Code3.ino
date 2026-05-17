#define F_CPU 16000000UL
#include <avr/io.h>
#include <util/delay.h>

#define ROTATION_DELAY 1000 
#define PULSE_MIN 1400  
#define PULSE_MID 3000   
#define PULSE_MAX 4600   

void timer_counter_1_init(void) {
    TCCR1A = (1 << WGM11);
    TCCR1B |= (1 << WGM12) | (1 << WGM13);
    
    TCCR1A |= (1 << COM1A1); 
    
    TCCR1B |= (1 << CS11); 
    
    ICR1 = 39999; 
}

int main(void) {
    DDRB |= (1 << PB5); 
    timer_counter_1_init();
    
    while (1) {
        OCR1A = PULSE_MIN;
        _delay_ms(ROTATION_DELAY);
        
        OCR1A = PULSE_MID; 
        _delay_ms(ROTATION_DELAY);
        
        OCR1A = PULSE_MAX; 
        _delay_ms(ROTATION_DELAY);
    }
    return 0;
}