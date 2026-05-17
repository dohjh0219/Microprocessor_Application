#define F_CPU 16000000UL
#include <avr/io.h>
#include <util/delay.h>
#include <stdlib.h>
#include <stdio.h>
#include "UART0.h" 

#define PULSE_MIN 1400 
#define PULSE_MAX 4600

void timer_counter_1_init(void) {
    TCCR1A = (1 << WGM11) | (1 << COM1A1);
    TCCR1B = (1 << WGM12) | (1 << WGM13) | (1 << CS11);
    ICR1 = 39999;
}

int main(void) {
    char rx_buf[10];
    int rx_idx = 0;
    
    DDRB |= (1 << PB5); 
    timer_counter_1_init();
    
    UART0_init();
    
    while (1) {
        char c = UART0_read(); 
        
        if (c == '\n' || c == '\r') {
            if (rx_idx > 0) {
                rx_buf[rx_idx] = '\0';
                int angle = atoi(rx_buf); 
                
                if (angle < 0) angle = 0;
                if (angle > 180) angle = 180;
                
                long pulse = PULSE_MIN + ((long)angle * (PULSE_MAX - PULSE_MIN)) / 180;
                OCR1A = (uint16_t)pulse;
                
                char msg[50];
                sprintf(msg, "Angle: %d\r\nPulse Width: %ld\r\n", angle, pulse);
                UART0_print(msg); 
                
                rx_idx = 0; 
            }
        } else {
            if (rx_idx < 9) {
                rx_buf[rx_idx++] = c;
            }
        }
    }
    return 0;
}