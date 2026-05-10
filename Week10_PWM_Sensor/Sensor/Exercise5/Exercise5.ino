#define F_CPU 16000000L
#include <avr/io.h>
#include "ADC.h"
#include "millis2560.h"

#define set_bit(value, bit) ( _SFR_BYTE(value) |= _BV(bit) )
#define clear_bit(value, bit) ( _SFR_BYTE(value) &= ~_BV(bit) )

#define READ_INTERVAL  200

#define THRESH_3LED   256
#define THRESH_2LED   512
#define THRESH_1LED   768

static uint8_t get_led_count(int adc) {
    if (adc < THRESH_3LED) return 4;
    if (adc < THRESH_2LED) return 3;
    if (adc < THRESH_1LED) return 2;
    return 1;
}

static void set_leds(uint8_t count) {
    PORTE &= ~((1 << PE4) | (1 << PE5) | (1 << PE3));
    PORTG &= ~(1 << PG5);

    if (count >= 1) PORTE |= (1 << PE4);
    if (count >= 2) PORTE |= (1 << PE5);
    if (count >= 3) PORTG |= (1 << PG5);
    if (count >= 4) PORTE |= (1 << PE3);
}

int main(void) {
    ADC_init(2, SINGLE_CONVERSION);
    millis2560_init();

    DDRE |= (1 << PE4) | (1 << PE5) | (1 << PE3);
    DDRG |= (1 << PG5);

    PORTE &= ~((1 << PE4) | (1 << PE5) | (1 << PE3));
    PORTG &= ~(1 << PG5);

    unsigned long last_read = 0;

    while (1) {
        unsigned long now = millis2560();

        if (now - last_read >= READ_INTERVAL) {
            last_read = now;

            int adc = read_ADC();
            set_leds(get_led_count(adc));
        }
    }

    return 0;
}
