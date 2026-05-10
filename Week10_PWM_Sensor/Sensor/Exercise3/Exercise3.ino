#define F_CPU 16000000L
#include <avr/io.h>
#include "UART0.h"
#include "ADC.h"
#include "millis2560.h"

#define set_bit(value, bit) ( _SFR_BYTE(value) |= _BV(bit) )
#define clear_bit(value, bit) ( _SFR_BYTE(value) &= ~_BV(bit) )

#define SENSOR_INTERVAL  1000
#define DEBOUNCE_TIME  50

int main(void) {
    UART0_init();
    ADC_init(1, SINGLE_CONVERSION);
    millis2560_init();

    clear_bit(DDRJ, 1);

    unsigned long last_sensor = 0;
    unsigned long last_btn_time = 0;
    uint8_t btn_prev = 0;
    uint8_t unit = 0;

    while (1) {
        unsigned long now = millis2560();

        uint8_t btn = (PINJ >> 1) & 1;
        if (btn && !btn_prev && (now - last_btn_time >= DEBOUNCE_TIME)) {
            unit = !unit;
            last_btn_time = now;
        }
        btn_prev = btn;

        if (now - last_sensor >= SENSOR_INTERVAL) {
            last_sensor = now;

            int read = read_ADC();
            int celsius = (int)(read * 500.0 / 1023.0);

            if (unit == 0) {
                UART0_print(celsius);
                UART0_write('C');
            } else {
                int fahrenheit = (int)(celsius * 9.0 / 5.0 + 32.0);
                UART0_print(fahrenheit);
                UART0_write('F');
            }
            UART0_write('\n');
        }
    }

    return 0;
}
