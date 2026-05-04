#define F_CPU 16000000L
#include <avr/io.h>
#include <avr/interrupt.h>

#define set_bit(value, bit) ( _SFR_BYTE(value) |= _BV(bit) )
#define clear_bit(value, bit) ( _SFR_BYTE(value) &= ~_BV(bit) )

volatile int count = 0;
uint8_t state = 0;

ISR(TIMER0_COMPA_vect) {
    TCNT0 = 0;
    count++;
    
    if (count == 64) { 
        count = 0;
        state = !state;
        if (state) set_bit(PORTE, 4);
        else clear_bit(PORTE, 4);
    }
}

void adc_init() {
    ADMUX = (1 << REFS0);
    ADCSRA = (1 << ADEN) | (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0);
}

uint16_t read_adc() {
    ADCSRA |= (1 << ADSC);
    while (ADCSRA & (1 << ADSC));
    return ADC;
}

int main(void) {
    set_bit(DDRB, 7);
    set_bit(DDRE, 4);
    
    clear_bit(PORTB, 7);
    clear_bit(PORTE, 4);

    adc_init(); 

    TCCR0B |= (1 << CS02) | (1 << CS00);
    TIMSK0 |= (1 << OCIE0A);
    OCR0A = 128;

    TCCR0A |= (1 << COM0A0); 

    sei();

    while (1) {
        uint16_t pot_value = read_adc(); 
        
        uint8_t new_ocr = (pot_value >> 2); 
        
        if (new_ocr < 10) new_ocr = 10; 

        cli(); 
        OCR0A = new_ocr;
        
        if (TCNT0 >= new_ocr) {
            TCNT0 = 0; 
        }
        sei();
    }
    return 0;
}