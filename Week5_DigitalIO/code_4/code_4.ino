#define F_CPU 16000000L

#include <avr/io.h>
#include <util/delay.h>

int main(void)
{
    init();
    Serial.begin(9600);

    uint8_t state_previous = 0;
    uint8_t state_current;
    int count = 0;

    DDRJ &= ~0x01;

    while (1)
    {
        state_current = PINJ & 0x01;

        if (state_current == 1)
        {
            if (state_previous == 0)
            {
                count++;
                Serial.println(count);
            }

            _delay_ms(50);
        }

        state_previous = state_current;
    }

    return 0;
}