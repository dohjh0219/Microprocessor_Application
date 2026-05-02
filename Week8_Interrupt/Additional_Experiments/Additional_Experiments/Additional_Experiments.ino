#define F_CPU 16000000UL
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <stdlib.h>
#include <util/atomic.h>

#define set_bit(value, bit) ( _SFR_BYTE(value) |= _BV(bit) )
#define clear_bit(value, bit) ( _SFR_BYTE(value) &= ~_BV(bit) )

void UART0_init(void) {
    UBRR0H = 0; UBRR0L = 103;
    UCSR0B = (1 << TXEN0);
    UCSR0C = (1 << UCSZ01) | (1 << UCSZ00);
}
void UART0_write(char data) {
    while (!(UCSR0A & (1 << UDRE0)));
    UDR0 = data;
}
void UART0_print_string(char* str) {
    while (*str) UART0_write(*str++);
}
void UART0_print(unsigned long num) {
    char buffer[15];
    ltoa(num, buffer, 10);
    UART0_print_string(buffer);
}

volatile unsigned long timer0_millis = 0;

ISR(TIMER0_COMPA_vect) {
    timer0_millis++; 
}

void Timer0_init(void) {
    TCCR0A = (1 << WGM01); 
    TCCR0B = (1 << CS01) | (1 << CS00);
    OCR0A = 249; 
    TIMSK0 = (1 << OCIE0A);
}

unsigned long get_millis(void) {
    unsigned long m;
    ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
        m = timer0_millis;
    }
    return m;
}

volatile int isr_count = 0;
volatile unsigned long last_interrupt_time = 0;

ISR(INT0_vect) {
    unsigned long current_time = get_millis();
    // 200ms 디바운싱
    if (current_time - last_interrupt_time > 200) { 
        if (isr_count < 5) {
            isr_count++;
            UART0_print_string(" -> [ISR] Hit: ");
            UART0_print(isr_count);
            UART0_write('\n');
        }
        last_interrupt_time = current_time;
    }
}

int main(void) {
    UART0_init();
    Timer0_init();
    
    clear_bit(DDRD, 0); 
    set_bit(PORTD, 0);

    sei();

    unsigned long start_time, end_time;
    
    UART0_print_string("\n=== [Phase 1] Polling Mode ===\n");
    UART0_print_string("Timer: 10 Seconds. Max Hits: 5\n");
    
    int polling_count = 0;
    uint8_t prev_state = 1;
    start_time = get_millis();

    while (get_millis() - start_time < 10000) {
        _delay_ms(400); 

        uint8_t curr_state = PIND & 0x01; 
        
        if (prev_state == 1 && curr_state == 0) {
            // 5회까지만 인정
            if (polling_count < 5) {
                polling_count++;
                UART0_print_string(" -> [Polling] Hit: ");
                UART0_print(polling_count);
                UART0_write('\n');
            }
        }
        prev_state = curr_state;
    }
    
    end_time = get_millis();
    UART0_print_string("\n>>> Phase 1 COMPLETE!\n");
    UART0_print_string(">>> Captured Hits: ");
    UART0_print(polling_count);
    UART0_print_string(" / 5\n");
    UART0_print_string(">>> Execution Time: ");
    UART0_print(end_time - start_time);
    UART0_print_string(" ms\n\n");

    UART0_print_string("Waiting 3 seconds before Phase 2...\n");
    _delay_ms(3000);

    UART0_print_string("\n=== [Phase 2] Interrupt Mode ===\n");
    UART0_print_string("Timer: 10 Seconds. Max Hits: 5\n");
    
    isr_count = 0;
    EIFR |= (1 << INTF0);
    EICRA |= (1 << ISC01);
    EIMSK |= (1 << INT0);
    
    start_time = get_millis();

    while (get_millis() - start_time < 10000) {
        _delay_ms(400);
    }
    
    end_time = get_millis();
    EIMSK &= ~(1 << INT0);
    
    UART0_print_string("\n>>> Phase 2 COMPLETE!\n");
    UART0_print_string(">>> Captured Hits: ");
    UART0_print(isr_count);
    UART0_print_string(" / 5\n");
    UART0_print_string(">>> Execution Time: ");
    UART0_print(end_time - start_time);
    UART0_print_string(" ms\n\n");
    
    UART0_print_string("=== ALL TESTS FINISHED ===\n");

    while (1) { } 
    return 0;
}