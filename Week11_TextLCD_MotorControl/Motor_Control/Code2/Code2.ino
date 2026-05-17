#define F_CPU 16000000UL
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

#define set_bit(value, bit) ( _SFR_BYTE(value) |= _BV(bit))
#define clear_bit(value, bit) ( _SFR_BYTE(value) &= ~_BV(bit))

#define motor_enable2()     PORTG |= 0x02
#define motor_disable2()    PORTG &= ~0x02
#define motor_forward2()    PORTG |= 0x01
#define motor_backward2()   PORTG &= ~0x01
#define motor_speed2(s)     OCR2A = s

void port_init() {
    set_bit(DDRG, 1);
    set_bit(DDRG, 0);
    set_bit(DDRB, 4);
    motor_disable2();
}

void timer_counter_2_init() {
    TCCR2A |= (1 << WGM21) | (1 << WGM20);
    TCCR2A |= (1 << COM2A1); 
    TCCR2B |= (1 << CS22) | (1 << CS21) | (1 << CS20); 
}

int main(void) {
    port_init();
    timer_counter_2_init();
    motor_enable2();
    
    while (1) {
        motor_forward2();
        for (int i = 255; i >= 0; i--) { 
            motor_speed2(i);
            _delay_ms(30);
        }
        _delay_ms(2000);
        
        motor_backward2();
        for (int i = 0; i < 256; i++) { 
            motor_speed2(i);
            _delay_ms(30);
        }
        _delay_ms(2000);
    }
    return 0;
}