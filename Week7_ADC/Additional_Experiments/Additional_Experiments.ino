#define F_CPU 16000000L
#include <avr/io.h>
#include <util/delay.h>
#include "UART0.h"

void UART0_print_string(const char *str) {
    while (*str) UART0_write(*str++); // null 만날 때 까지, 문자열 한 글자씩 전송
}

void init_ADC_game() {
    ADMUX = (1 << REFS0); // ref voltage 5v
    ADCSRA = (1 << ADEN) | (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0);
    // adc enable, prescaler 128 set(125kHz)
}

int read_ADC_A0() {
    ADMUX = (ADMUX & 0xF0); // a0 input select
    ADCSRA |= (1 << ADSC); // start conv

    while (ADCSRA & (1 << ADSC)); // wait adc conv end
    return ADC;
}

int run_player_turn(int player_num) {
    UART0_print_string("\n>>> PLAYER ");
    UART0_print(player_num);
    UART0_print_string(" START! <<<\n");

    for (int i = 3; i > 0; i--) {
        UART0_print(i); UART0_print_string("... ");
        _delay_ms(1000);
    }

    UART0_print_string("GO!!\n");

    int final_val = 0;

    for (int t = 100; t >= 0; t--) {
        final_val = read_ADC_A0();

        UART0_print_string("\nTime: ");
        UART0_print(t / 10); 
        UART0_print_string(".");
        UART0_print(t % 10); 
        UART0_print_string("s");
        UART0_print_string("      "); 
        _delay_ms(100);
    }
    
    UART0_print_string("\nSTOP!! Final Value: ");
    UART0_print(final_val);
    UART0_print_string("\n");

    return final_val;
}

int main(void) {
    UART0_init();
    init_ADC_game();
    UART0_print_string("=== 10s Potentiometer Challenge ===\n");
    UART0_print_string("Target: 500 | Closest wins!\n");

    int p1_val = run_player_turn(1);
    int p1_error = (p1_val > 500) ? (p1_val - 500) : (500 - p1_val);

    UART0_print_string("\nReset dial to 0 for P2...\n");

    while (read_ADC_A0() > 20) _delay_ms(100);

    int p2_val = run_player_turn(2);
    int p2_error = (p2_val > 500) ? (p2_val - 500) : (500 - p2_val);

    UART0_print_string("\n========== RESULT ==========\n");
    UART0_print_string("P1 Error: "); UART0_print(p1_error);      
    UART0_print_string("\n");
    UART0_print_string("P2 Error: "); UART0_print(p2_error); 
    UART0_print_string("\n");

    if (p1_error < p2_error) {
        UART0_print_string("WINNER: PLAYER 1 !!!\n");
    } else if (p2_error < p1_error) {
        UART0_print_string("WINNER: PLAYER 2 !!!\n");
    } else {
        UART0_print_string("DRAW!\n");
    }

    UART0_print_string("============================\n");
    while (1);

    return 0;
}


