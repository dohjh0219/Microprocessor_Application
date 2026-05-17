#define F_CPU 16000000UL

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include "Text_LCD.h"

#define BAUD     9600UL
#define UBRR_VAL (F_CPU / 16 / BAUD - 1)

#define TARGET_DIST_CM 100000L

void UART_init(void) {
    UBRR0H = (uint8_t)(UBRR_VAL >> 8);
    UBRR0L = (uint8_t)(UBRR_VAL);
    UCSR0B = (1 << RXEN0) | (1 << TXEN0);
    UCSR0C = (1 << UCSZ01) | (1 << UCSZ00);
}

static uint8_t UART_rx(void) {
    while (!(UCSR0A & (1 << RXC0)));
    return UDR0;
}

int16_t UART_receive_speed(void) {
    char    buf[8];
    uint8_t i = 0;
    char    c;

    while (1) {
        c = (char)UART_rx();
        if (c == '\n' || c == '\r') {
            if (i > 0) break;
            continue; 
        }
        if (i < (uint8_t)(sizeof(buf) - 1)) buf[i++] = c;
    }
    buf[i] = '\0';

    int16_t val = 0;
    for (uint8_t j = 0; j < i; j++) {
        if (buf[j] >= '0' && buf[j] <= '9')
            val = val * 10 + (buf[j] - '0');
    }
    return val;
}

volatile uint32_t g_ovf = 0;

ISR(TIMER1_OVF_vect) { g_ovf++; }

void timer1_init(void) {
    TCCR1A = 0x00;
    TCCR1B = (1 << CS12) | (1 << CS10);  
    TIMSK1 = (1 << TOIE1);
    sei();
}

static uint32_t elapsed_ms(void) {
    cli();
    uint32_t ovf = g_ovf;
    uint16_t cnt = TCNT1;
    sei();
    return (ovf * 65536UL + (uint32_t)cnt) * 64UL / 1000UL;
}

static void timer1_reset(void) {
    cli();
    g_ovf = 0;
    TCNT1 = 0;
    sei();
}


static void LCD_puts(const char *s) {
    while (*s) LCD_write_data((uint8_t)*s++);
}

static void LCD_write_padded(uint32_t val, uint8_t width) {
    char    buf[6];
    uint8_t len = 0;

    if (val == 0) {
        buf[len++] = '0';
    } else {
        while (val > 0 && len < (uint8_t)sizeof(buf)) {
            buf[len++] = '0' + (uint8_t)(val % 10);
            val /= 10;
        }
    }
    // 남은 자리 공백 패딩
    for (uint8_t p = len; p < width; p++) LCD_write_data(' ');
    // 역순 출력
    while (len > 0) LCD_write_data((uint8_t)buf[--len]);
}


static void LCD_update(int16_t speed_kmh, int32_t remaining_cm) {
    if (remaining_cm < 0) remaining_cm = 0;

    uint32_t m_int = (uint32_t)(remaining_cm / 100); 
    uint8_t  m_dec = (uint8_t)((remaining_cm % 100) / 10); 

    // Row 0
    LCD_goto_XY(0, 0);
    LCD_puts("Speed: ");         
    LCD_write_padded((uint32_t)speed_kmh, 3); 
    LCD_puts(" km/h ");

    // Row 1
    LCD_goto_XY(1, 0);
    LCD_puts("Dist: ");  
    LCD_write_padded(m_int, 4); 
    LCD_write_data('.');
    LCD_write_data('0' + m_dec);  
    LCD_puts(" m  ");    
}

int main(void) {
    LCD_init();
    UART_init();
    timer1_init();

    int32_t remaining_cm = TARGET_DIST_CM;
    int16_t speed_kmh    = 0;
    uint8_t first        = 1; 

    LCD_clear();
    LCD_update(0, remaining_cm);

    while (1) {
        speed_kmh = UART_receive_speed();

        if (first) {
            first = 0;
        } else {
            uint32_t dt_ms = elapsed_ms();

            if (dt_ms > 60000UL) dt_ms = 60000UL;

            int32_t moved_cm = (int32_t)speed_kmh * (int32_t)dt_ms / 36L;
            remaining_cm    -= moved_cm;
            if (remaining_cm < 0) remaining_cm = 0;
        }

        timer1_reset();
        LCD_update(speed_kmh, remaining_cm);
    }

    return 0;
}
