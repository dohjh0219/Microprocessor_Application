#define F_CPU 16000000UL
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

#define set_bit(value, bit) ( _SFR_BYTE(value) |= _BV(bit))
#define clear_bit(value, bit) ( _SFR_BYTE(value) &= ~_BV(bit))

#define motor_enable1()     PORTD |= 0x80
#define motor_disable1()    PORTD &= ~0x80
#define motor_forward1()    PORTG |= 0x04
#define motor_backward1()   PORTG &= ~0x04
#define motor_speed1(s)     OCR2B = s

void port_init() {
    set_bit(DDRD, 7);
    set_bit(DDRG, 2); 
    set_bit(DDRH, 6);
    motor_disable1();
}

void timer_counter_2_init() {
    TCCR2A |= (1 << WGM21) | (1 << WGM20);
    TCCR2A |= (1 << COM2B1); 
    
    TCCR2B |= (1 << CS22) | (1 << CS21) | (1 << CS20); 
    OCR2B = 0;
}

int main(void) {
    port_init();
    timer_counter_2_init();
    
    while (1) {
        motor_forward1();
        motor_enable1();
        motor_speed1(0); 
        _delay_ms(3000);
        
        motor_disable1();
        _delay_ms(1000);
        
        motor_backward1();
        motor_enable1();
        motor_speed1(255); 
        _delay_ms(3000);
        
        motor_disable1();
        _delay_ms(1000);
    }
    return 0;
}