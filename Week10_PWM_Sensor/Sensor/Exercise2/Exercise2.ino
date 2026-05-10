#define F_CPU 16000000L
#include <avr/io.h>
#include "UART0.h"
#include "ADC.h"
#include "millis2560.h"

#define set_bit(value, bit) ( _SFR_BYTE(value) |= _BV(bit) )
#define clear_bit(value, bit) ( _SFR_BYTE(value) &= ~_BV(bit) )

#define THRESHOLD 42
#define SENSOR_INTERVAL  1000
#define BLINK_INTERVAL    500

int main(void) {
    UART0_init();
    ADC_init(1, SINGLE_CONVERSION);
    millis2560_init();

    set_bit(DDRB, 7);
    clear_bit(PORTB, 7);

    unsigned long last_sensor = 0;
    unsigned long last_blink  = 0;
    uint8_t led_state   = 0;
    int temperature     = 0;

    while (1) {
        unsigned long now = millis2560();

        if (now - last_sensor >= SENSOR_INTERVAL) {
            last_sensor = now;

            int read = read_ADC();
            temperature = (int)(read * 500.0 / 1023.0);

            if (temperature < THRESHOLD) {
                clear_bit(PORTB, 7);
                led_state = 0;
            }

            UART0_print(temperature);
            UART0_write('\n');
        }

        if (temperature >= THRESHOLD) {
            if (now - last_blink >= BLINK_INTERVAL) {
                last_blink = now;
                led_state = !led_state;
                if (led_state) set_bit(PORTB, 7);
                else clear_bit(PORTB, 7);
            }
        }
    }

    return 0;
}
