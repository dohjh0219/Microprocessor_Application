#define F_CPU 16000000L
#include <avr/io.h>
#include <util/delay.h>
#include <Arduino.h>

int main(void)
{
    init();
    Serial.begin(9600);

    DDRE |= 0x38;
    DDRG |= 0x20;

    DDRJ &= ~0x03;
    DDRH &= ~0x02;

    while (1)
    {
        PORTE &= ~0x38;
        PORTG &= ~0x20;

        Serial.println("\n===========================");
        Serial.println("      F1 REACTION GAME     ");
        Serial.println("===========================");
        Serial.println("Press the Start Button");
        
        while (((PINJ >> 1) & 0x01) == 0) {
        }
        
        while (((PINJ >> 1) & 0x01) == 1) {
            delay(10);
        }

        Serial.println("Get Ready...");
        
        while ((PINJ & 0x01) == 1 || ((PINH >> 1) & 0x01) == 0) {
        }

        delay(1000);

        PORTE |= 0x10; delay(1000);
        PORTE |= 0x20; delay(1000);
        PORTG |= 0x20; delay(1000);
        PORTE |= 0x08;

        randomSeed(millis());
        int random_wait = random(1000, 4000); 
        delay(random_wait);

        PORTE &= ~0x38;
        PORTG &= ~0x20;
        Serial.println("GO!!!");

        unsigned long start_time = millis(); 
        
        unsigned long left_time = 0;
        unsigned long right_time = 0;
        bool left_pressed = false;
        bool right_pressed = false;

        while (!left_pressed || !right_pressed)
        {
            if (!left_pressed && ((PINJ & 0x01) == 1)) {
                left_time = millis() - start_time;
                left_pressed = true;
            } 
            if (!right_pressed && (((PINH >> 1) & 0x01) == 0)) {
                right_time = millis() - start_time;
                right_pressed = true;
            }
        }

        Serial.println("\n--- RESULT ---");
        Serial.print("Left  Time: "); Serial.print(left_time); Serial.println(" ms");
        Serial.print("Right Time: "); Serial.print(right_time); Serial.println(" ms");

        if (left_time < right_time) {
            Serial.println(">> LEFT WINS! <<");
            for (int i = 0; i < 5; i++) {
                PORTE |= 0x30; delay(100);
                PORTE &= ~0x30; delay(100);
            }
        } 
        else if (right_time < left_time) {
            Serial.println(">> RIGHT WINS! <<");
            for (int i = 0; i < 5; i++) {
                PORTG |= 0x20; PORTE |= 0x08; delay(100);
                PORTG &= ~0x20; PORTE &= ~0x08; delay(100);
            }
        }
        else {
            Serial.println(">> TIE! (DRAW) <<");
            for (int i = 0; i < 5; i++) {
                PORTE |= 0x38; PORTG |= 0x20; delay(100);
                PORTE &= ~0x38; PORTG &= ~0x20; delay(100);
            }
        }
    }

    return 0;
}