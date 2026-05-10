#define F_CPU 16000000L
#include <avr/io.h>
#include <util/delay.h>

#define set_bit(value, bit) ( _SFR_BYTE(value) |= _BV(bit) )
#define clear_bit(value, bit) ( _SFR_BYTE(value) &= ~_BV(bit) )

void adc_init(void) {
    ADMUX = (1 << REFS0);
    ADCSRA = (1 << ADEN) | (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0);
}

uint16_t adc_read(void) {
    ADCSRA |= (1 << ADSC);
    while (ADCSRA & (1 << ADSC));
    return ADC;
}

int main(void) {
    set_bit(DDRB, 7);

    TCCR0A |= (1 << WGM00) | (1 << COM0A1);
    TCCR0B |= (1 << CS02);

    adc_init();

    while (1) {
        uint16_t adc_val = adc_read();
        OCR0A = adc_val >> 2;
    }

    return 0;
}
