#define F_CPU 16000000UL
#include <avr/io.h>
#include <util/delay.h>
#include "ADC.h"

#define set_bit(value, bit) ( _SFR_BYTE(value) |= _BV(bit))
#define clear_bit(value, bit) ( _SFR_BYTE(value) &= ~_BV(bit))

#define motor_enable1()     PORTD |= 0x80
#define motor_forward1()    PORTG |= 0x04

void port_init() {
    set_bit(DDRD, 7); 
    set_bit(DDRG, 2); 
    set_bit(DDRH, 6);
}

void timer_counter_2_init() {
    TCCR2A |= (1 << WGM21) | (1 << WGM20);
    TCCR2A |= (1 << COM2B1);
    TCCR2B |= (1 << CS22) | (1 << CS21) | (1 << CS20);
    OCR2B = 255; 
}

int main(void) {
    port_init();
    timer_counter_2_init();
    
    ADC_init(0, FREE_RUNNING); 
    
    motor_forward1();
    motor_enable1();
    
    while (1) {
        int adc_value = read_ADC(); 
        
        int pwm_value = 255 - (adc_value / 4);
        
        OCR2B = pwm_value;
        _delay_ms(10);
    }
    return 0;
}