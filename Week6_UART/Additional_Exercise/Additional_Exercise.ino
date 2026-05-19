#include <avr/io.h>
#include <util/delay.h>
#include <Arduino.h>
#include "UART0.h"

void println(const char *s) {
    UART0_print((char*)s);
    UART0_print((char*)"\r\n");
}

int read_num(void) {
    char buf[8], c, i = 0;
    while (1) {
        c = UART0_read();
        UART0_write(c);
        
        if (c == '\r' || c == '\n') {
            UART0_print((char*)"\r\n");
            buf[i] = '\0';
            return atoi(buf);
        }
        
        if (c >= '0' && c <= '9' && i < 7) {
            buf[i++] = c;
        }
    }
}

void led_on(void)  { PORTE |= 0x38; PORTG |= 0x20; }
void led_off(void) { PORTE &= ~0x38; PORTG &= ~0x20; }


void blink_left_leds(void) {
    for (int i = 0; i < 5; i++) { 
        PORTE |= 0x30;  
        _delay_ms(200);
        PORTE &= ~0x30;
        _delay_ms(200);
    }
}

void blink_right_leds(void) {
    for (int i = 0; i < 5; i++) { 
        PORTG |= 0x20;
        PORTE |= 0x08;
        _delay_ms(200);
        PORTG &= ~0x20;
        PORTE &= ~0x08;
        _delay_ms(200);
    }
}

void blink_all_leds(void) {
    for (int i = 0; i < 5; i++) {
        led_on();
        _delay_ms(200);
        led_off();
        _delay_ms(200);
    }
}

int main(void) {
    init();
    UART0_init();

    DDRE |= 0x38;
    DDRG |= 0x20;
    DDRJ &= ~0x03;
    DDRH &= ~0x02;

    while (1) {
        int sec;
        unsigned long target_ms, start_time;
        unsigned long left_result = 0, right_result = 0;
        long left_error, right_error;
        bool left_done = false, right_done = false;

        led_off();
        println("\n[ 2-PLAYER TIMING GAME ]");
        println("Input target seconds (e.g. 5):");
        
        sec = read_num();
        if (sec <= 0) continue;
        target_ms = (unsigned long)sec * 1000UL;

        println("Press START Button (PJ1)");
        
        while (((PINJ >> 1) & 0x01) == 1);
        while (((PINJ >> 1) & 0x01) == 0);

        _delay_ms(1000);
        led_on();
        println("Ready...");
        _delay_ms(3000);
        led_off();
        println("START! Count in your head!");

        start_time = millis();

        while (!left_done || !right_done) {
            if (!left_done && (PINJ & 0x01)) {
                left_result = millis() - start_time;
                left_done = true;
            }
            if (!right_done && (((PINH >> 1) & 0x01) == 0)) {
                right_result = millis() - start_time;
                right_done = true;
            }
        }

        left_error = (long)left_result - (long)target_ms;
        right_error = (long)right_result - (long)target_ms;

        println("\n--- GAME RESULT ---");
        
        UART0_print((char*)"Target: "); 
        UART0_print((long)target_ms); 
        println(" ms");
        
        UART0_print((char*)"Left : "); 
        UART0_print((long)left_result); 
        UART0_print((char*)" ms (Error: "); 
        UART0_print(left_error); 
        println(" ms)");
        
        UART0_print((char*)"Right: "); 
        UART0_print((long)right_result); 
        UART0_print((char*)" ms (Error: "); 
        UART0_print(right_error); 
        println(" ms)");

        if (labs(left_error) < labs(right_error)) {
            println(">> LEFT PLAYER WINS! <<");
            blink_left_leds();
        } else if (labs(right_error) < labs(left_error)) {
            println(">> RIGHT PLAYER WINS! <<");
            blink_right_leds();
        } else {
            println(">> DRAW! <<");
            blink_all_leds();
        }
        println("\nReady for Next Game...");
    }
    return 0;
}
