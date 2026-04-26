#include "ADC.h"

static int current_mode = SINGLE_CONVERSION;

void ADC_init(unsigned char channel, int mode) {
    current_mode = mode;

    ADMUX |= (1 << REFS0);

    ADMUX = (ADMUX & 0xE0) | (channel & 0x07);
    if (channel >= 8) {
        ADCSRB |= (1 << MUX5);
    } else {
        ADCSRB &= ~(1 << MUX5);
    }

    ADCSRA |= (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0); // 128분주비

    if (mode == FREE_RUNNING) {
        ADCSRA |= (1 << ADATE);  // auto trigger enable
        ADCSRB &= ~((1 << ADTS2) | (1 << ADTS1) | (1 << ADTS0)); // free running mode
        ADCSRA |= (1 << ADEN);  // ADC enable
        ADCSRA |= (1 << ADSC);  // start conversion
    } else {
        ADCSRA &= ~(1 << ADATE); // auto trigger disable
        ADCSRA |= (1 << ADEN);  // ADC enable
    }
}

int read_ADC(void) {
    if (current_mode == SINGLE_CONVERSION) {
        ADCSRA |= (1 << ADSC);
    }

    while (!(ADCSRA & (1 << ADIF)));

    return ADC;
}