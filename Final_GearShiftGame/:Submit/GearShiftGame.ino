#define F_CPU 16000000UL

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include "Text_LCD.h"

#define set_bit(r,b)   (_SFR_BYTE(r) |=  _BV(b))
#define clear_bit(r,b) (_SFR_BYTE(r) &= ~_BV(b))

#define BTN_CLUTCH  0   
#define BTN_UP      1  
#define BTN_ACCEL   2  
#define BTN_COUNT   3

#define motor_enable()   (PORTD |=  0x80)
#define motor_disable()  (PORTD &= ~0x80)
#define motor_forward()  (PORTG |=  0x04)
#define motor_set_spd(s) (OCR2B  = (s))

#define SERVO_MIN  1400U   
#define SERVO_MAX  4600U   

#define TOTAL_DIST_M    400.0f
#define DT_SEC           0.05f
#define MAX_RPM         7000.0f
#define IDLE_RPM         900.0f
#define OPTIMAL_RPM_LO  3000.0f
#define OPTIMAL_RPM_HI  5500.0f
#define REDLINE_RPM     6800.0f
#define DRAG_K          0.0004f
#define CLUTCH_RPM_FALL 120.0f

static const float MAX_SPD[7]    = { 0.0f, 40.0f, 75.0f, 110.0f, 150.0f, 185.0f, 220.0f };
static const float BASE_ACCEL[7] = { 0.0f, 11.0f,  8.5f,   6.0f,   4.0f,   3.0f,   2.2f };
static const float RPM_SCALE[7]  = { 0.0f, 175.0f, 93.3f,  63.6f,  46.7f,  36.8f,  30.9f };

typedef enum {
    PHASE_INTRO = 0,
    PHASE_P1_READY,
    PHASE_P1_PLAY,
    PHASE_P1_FINISH,
    PHASE_P2_READY,
    PHASE_P2_PLAY,
    PHASE_RESULT
} GamePhase;

typedef struct {
    float    speed;
    float    dist;
    float    rpm;
    int8_t   gear;
    int8_t   next_gear;
    uint8_t  clutch_in;
    uint32_t time_ms;
} CarState;

static GamePhase g_phase = PHASE_INTRO;
static CarState  g_car[2];
static uint8_t   g_cur = 0;

static volatile uint32_t g_ovf = 0;

ISR(TIMER3_OVF_vect) { g_ovf++; }

static void msec_init(void) {
    TCCR3A = 0x00;
    TCCR3B = (1 << CS32) | (1 << CS30);
    TIMSK3 = (1 << TOIE3);
    sei();
}

static void msec_reset(void) {
    cli(); g_ovf = 0; TCNT3 = 0; sei();
}

static uint32_t elapsed_ms(void) {
    cli();
    uint32_t ovf = g_ovf;
    uint16_t cnt = TCNT3;
    sei();
    return (ovf * 65536UL + (uint32_t)cnt) * 64UL / 1000UL;
}

static void motor_init(void) {
    set_bit(DDRD, 7);
    set_bit(DDRG, 2);
    set_bit(DDRH, 6);
    motor_forward();
    motor_disable();
    TCCR2A |= (1 << WGM21) | (1 << WGM20) | (1 << COM2B1);
    TCCR2B |= (1 << CS22)  | (1 << CS21)  | (1 << CS20);
    OCR2B   = 0;
}

static void motor_update(float rpm) {
    float r = (rpm - IDLE_RPM) / (MAX_RPM - IDLE_RPM);
    if (r < 0.0f) r = 0.0f;
    if (r > 1.0f) r = 1.0f;
    motor_set_spd((uint8_t)(230.0f * (1.0f - r)));
}

static void servo1_init(void) {
    set_bit(DDRB, 5);
    TCCR1A  = (1 << WGM11) | (1 << COM1A1);
    TCCR1B  = (1 << WGM13) | (1 << WGM12);
    ICR1    = 39999;
    OCR1A   = SERVO_MIN;
    TCCR1B |= (1 << CS11);
}

static void servo1_update(float rpm) {
    if (rpm < IDLE_RPM)    rpm = IDLE_RPM;
    if (rpm > REDLINE_RPM) rpm = REDLINE_RPM;
    float r = (rpm - IDLE_RPM) / (REDLINE_RPM - IDLE_RPM);
    OCR1A = (uint16_t)(SERVO_MIN + r * (SERVO_MAX - SERVO_MIN));
}

static void servo2_init(void) {
    set_bit(DDRH, 4);
    TCCR4A  = (1 << WGM41) | (1 << COM4B1);
    TCCR4B  = (1 << WGM43) | (1 << WGM42);
    ICR4    = 39999;
    OCR4B   = SERVO_MIN;
    TCCR4B |= (1 << CS41);
}

#define SERVO2_OFFSET  300U  

static void servo2_update(float speed) {
    if (speed < 0.0f)       speed = 0.0f;
    if (speed > MAX_SPD[6]) speed = MAX_SPD[6];
    float r = speed / MAX_SPD[6];
    OCR4B = (uint16_t)(SERVO_MIN + r * (SERVO_MAX - SERVO_MIN)) + SERVO2_OFFSET;
}

#define UBRR_VAL (F_CPU / 16 / 9600UL - 1)

static void uart_init(void) {
    UBRR0H = (uint8_t)(UBRR_VAL >> 8);
    UBRR0L = (uint8_t) UBRR_VAL;
    UCSR0B = (1 << TXEN0);
    UCSR0C = (1 << UCSZ01) | (1 << UCSZ00);
}

static void uart_putc(char c) {
    while (!(UCSR0A & (1 << UDRE0)));
    UDR0 = (uint8_t)c;
}

static void uart_puts(const char *s) { while (*s) uart_putc(*s++); }

static void uart_putuint(uint32_t v) {
    char buf[6]; uint8_t i = 0;
    if (v == 0) { uart_putc('0'); return; }
    while (v && i < 5) { buf[i++] = '0' + (uint8_t)(v % 10); v /= 10; }
    while (i) uart_putc(buf[--i]);
}

static void uart_send(CarState *c) {
    uart_puts("S:"); uart_putuint((uint32_t)c->speed);
    uart_puts(",R:"); uart_putuint((uint32_t)c->rpm);
    uart_puts(",G:"); uart_putc('0' + (uint8_t)(c->gear > 0 ? c->gear : 0));
    uart_putc('\n');
}

static uint8_t  btn_stable[BTN_COUNT];
static uint8_t  btn_last_raw[BTN_COUNT];
static uint8_t  btn_edge[BTN_COUNT];
static uint32_t btn_debounce_ts[BTN_COUNT];
#define DEBOUNCE_MS 25

static void buttons_init(void) {
    DDRJ &= ~0x03;
    clear_bit(DDRD, 0);
    set_bit(PORTD, 0);
    for (uint8_t i = 0; i < BTN_COUNT; i++)
        btn_stable[i] = btn_last_raw[i] = btn_edge[i] = btn_debounce_ts[i] = 0;
}

static uint8_t btn_read_raw(uint8_t i) {
    switch (i) {
    case BTN_CLUTCH: return  (PINJ & 0x01);
    case BTN_UP:     return  (PINJ >> 1) & 0x01;
    case BTN_ACCEL:  return !(PIND & 0x01);
    default:         return 0;
    }
}

static void buttons_update(uint32_t now_ms) {
    for (uint8_t i = 0; i < BTN_COUNT; i++) {
        uint8_t r = btn_read_raw(i);
        if (r != btn_last_raw[i]) {
            btn_debounce_ts[i] = now_ms;
            btn_last_raw[i]    = r;
        }
        if ((now_ms - btn_debounce_ts[i]) >= DEBOUNCE_MS) {
            if (r && !btn_stable[i]) btn_edge[i] = 1;
            btn_stable[i] = r;
        }
    }
}

#define BTN(i) (btn_stable[i])

static uint8_t btn_consume_edge(uint8_t i) {
    uint8_t e = btn_edge[i]; btn_edge[i] = 0; return e;
}
#define BTN_EDGE(i) btn_consume_edge(i)

static float torque_eff(float rpm) {
    if (rpm < IDLE_RPM)                                  return 0.05f;
    if (rpm >= REDLINE_RPM)                              return 0.0f;
    if (rpm >= OPTIMAL_RPM_LO && rpm <= OPTIMAL_RPM_HI) return 1.0f;
    if (rpm < OPTIMAL_RPM_LO) {
        float t = (rpm - IDLE_RPM) / (OPTIMAL_RPM_LO - IDLE_RPM);
        return 0.2f + 0.8f * t;
    }
    return 1.0f - (rpm - OPTIMAL_RPM_HI) / (REDLINE_RPM - OPTIMAL_RPM_HI);
}

static float speed_to_rpm(float spd_kmh, int8_t gear) {
    if (gear <= 0) return IDLE_RPM;
    float rpm = spd_kmh * RPM_SCALE[gear];
    return rpm < IDLE_RPM ? IDLE_RPM : rpm;
}

static void car_update(CarState *c) {
    uint8_t clutch = BTN(BTN_CLUTCH);
    uint8_t accel  = BTN(BTN_ACCEL);

    if (!c->clutch_in && clutch)
        c->next_gear = c->gear;

    if (clutch && BTN_EDGE(BTN_UP) && c->next_gear < 6)
        c->next_gear++;

    if (c->clutch_in && !clutch) {
        c->gear      = c->next_gear;
        c->next_gear = 0;
        c->rpm       = speed_to_rpm(c->speed, c->gear);
    }
    c->clutch_in = clutch;

    if (clutch) {
        c->rpm -= CLUTCH_RPM_FALL;
        if (c->rpm < IDLE_RPM) c->rpm = IDLE_RPM;
    } else {
        c->rpm = speed_to_rpm(c->speed, c->gear);
    }

    float a    = 0.0f;
    float v_ms = c->speed / 3.6f;

    if (accel && !clutch && c->gear > 0 && c->rpm < REDLINE_RPM)
        a += BASE_ACCEL[c->gear] * torque_eff(c->rpm);

    a -= DRAG_K * v_ms * v_ms;

    c->speed += a * DT_SEC * 3.6f;
    if (c->speed < 0.0f) c->speed = 0.0f;
    c->dist += (c->speed / 3.6f) * DT_SEC;
}

static void LCD_puts(const char *s) {
    while (*s) LCD_write_data((uint8_t)*s++);
}

static void LCD_uint(uint32_t v, uint8_t w) {
    char buf[6]; uint8_t n = 0;
    if (v == 0) buf[n++] = '0';
    else while (v && n < 5) { buf[n++] = '0' + (uint8_t)(v % 10); v /= 10; }
    for (uint8_t p = n; p < w; p++) LCD_write_data(' ');
    while (n) LCD_write_data((uint8_t)buf[--n]);
}

static void scr_intro(void) {
    LCD_clear();
    LCD_goto_XY(0, 0); LCD_puts("GEAR SHIFT RACE!");
    LCD_goto_XY(1, 0); LCD_puts("Press any button");
}

static void scr_ready(uint8_t p) {
    LCD_clear();
    LCD_goto_XY(0, 0); LCD_puts(p == 0 ? "Player 1 Ready! " : "Player 2 Ready! ");
    LCD_goto_XY(1, 0); LCD_puts("  Press ACCEL!  ");
}

static void scr_finish(uint8_t p, uint32_t ms) {
    LCD_clear();
    LCD_goto_XY(0, 0); LCD_puts(p == 0 ? "P1 Finish! " : "P2 Finish! ");
    uint32_t s  = ms / 1000;
    uint8_t  cs = (uint8_t)((ms % 1000) / 10);
    LCD_uint(s, 2); LCD_write_data('.');
    if (cs < 10) LCD_write_data('0');
    LCD_uint(cs, 2); LCD_write_data('s');
    LCD_goto_XY(1, 0); LCD_puts("  Great run!!   ");
}

static void scr_result(void) {
    LCD_clear();
    LCD_goto_XY(0, 0);
    if      (g_car[0].time_ms < g_car[1].time_ms) LCD_puts("Player 1 WINS!! ");
    else if (g_car[1].time_ms < g_car[0].time_ms) LCD_puts("Player 2 WINS!! ");
    else                                           LCD_puts("  DRAW!!        ");
    LCD_goto_XY(1, 0);
    LCD_puts("P1:");
    uint32_t s1 = g_car[0].time_ms / 1000;
    uint8_t  c1 = (uint8_t)((g_car[0].time_ms % 1000) / 10);
    LCD_uint(s1, 2); LCD_write_data('.');
    if (c1 < 10) LCD_write_data('0'); LCD_uint(c1, 2);
    LCD_puts(" P2:");
    uint32_t s2 = g_car[1].time_ms / 1000;
    uint8_t  c2 = (uint8_t)((g_car[1].time_ms % 1000) / 10);
    LCD_uint(s2, 2); LCD_write_data('.');
    if (c2 < 10) LCD_write_data('0'); LCD_uint(c2, 2);
}

static void scr_play(CarState *c, uint8_t p) {
    LCD_goto_XY(0, 0);
    float progress = c->dist / TOTAL_DIST_M;
    if (progress > 1.0f) progress = 1.0f;
    uint8_t filled = (uint8_t)(progress * 16.0f);
    for (uint8_t i = 0; i < 16; i++)
        LCD_write_data(i < filled ? '#' : '-');

    LCD_goto_XY(1, 0);
    LCD_write_data('P'); LCD_write_data('1' + p); LCD_write_data(' ');
    float remain = TOTAL_DIST_M - c->dist;
    if (remain < 0.0f) remain = 0.0f;
    LCD_uint((uint32_t)remain, 3);
    LCD_write_data('m'); LCD_write_data(' ');
    LCD_puts("G:");
    int8_t dg = c->clutch_in ? c->next_gear : c->gear;
    LCD_write_data(dg > 0 ? ('0' + (uint8_t)dg) : 'N');
    LCD_puts("     ");
}

static void car_reset(uint8_t i) {
    g_car[i].speed     = 0.0f;
    g_car[i].dist      = 0.0f;
    g_car[i].rpm       = IDLE_RPM;
    g_car[i].gear      = 1;
    g_car[i].next_gear = 0;
    g_car[i].clutch_in = 0;
    g_car[i].time_ms   = 0;
}

static void outputs_reset(void) {
    motor_set_spd(255); 
    OCR1A = SERVO_MIN;
    OCR4B = SERVO_MIN;
}

int main(void) {
    LCD_init();
    motor_init();
    servo1_init();
    servo2_init();
    buttons_init();
    uart_init();
    msec_init();

    motor_enable();
    car_reset(0);
    car_reset(1);
    g_phase = PHASE_INTRO;
    scr_intro();

    uint32_t last_physics = 0;
    uint32_t last_lcd     = 0;
    uint32_t finish_ts    = 0;

    while (1) {
        uint32_t now = elapsed_ms();
        buttons_update(now);

        switch (g_phase) {

        case PHASE_INTRO:
            for (uint8_t i = 0; i < BTN_COUNT; i++) {
                if (BTN_EDGE(i)) {
                    car_reset(0); car_reset(1);
                    g_phase = PHASE_P1_READY;
                    scr_ready(0);
                    break;
                }
            }
            break;

        case PHASE_P1_READY:
            if (BTN_EDGE(BTN_ACCEL)) {
                g_cur = 0;
                msec_reset();
                last_physics = last_lcd = 0;
                g_phase = PHASE_P1_PLAY;
                LCD_clear();
            }
            break;

        case PHASE_P1_PLAY:
        case PHASE_P2_PLAY: {
            CarState *c = &g_car[g_cur];

            if ((now - last_physics) >= 50) {
                last_physics = now;
                car_update(c);
                motor_update(c->rpm);
                servo1_update(c->rpm);
                servo2_update(c->speed);
                uart_send(c);

                if (c->dist >= TOTAL_DIST_M) {
                    c->time_ms = elapsed_ms();
                    outputs_reset();
                    scr_finish(g_cur, c->time_ms);
                    finish_ts = elapsed_ms();
                    g_phase = (g_cur == 0) ? PHASE_P1_FINISH : PHASE_RESULT;
                    if (g_phase == PHASE_RESULT) {
                        _delay_ms(2500);
                        scr_result();
                    }
                }
            }

            if ((now - last_lcd) >= 150) {
                last_lcd = now;
                scr_play(c, g_cur);
            }
            break;
        }

        case PHASE_P1_FINISH:
            if ((elapsed_ms() - finish_ts) >= 3000) {
                g_phase = PHASE_P2_READY;
                scr_ready(1);
            }
            break;

        case PHASE_P2_READY:
            if (BTN_EDGE(BTN_ACCEL)) {
                g_cur = 1;
                msec_reset();
                last_physics = last_lcd = 0;
                g_phase = PHASE_P2_PLAY;
                LCD_clear();
            }
            break;

        case PHASE_RESULT:
            for (uint8_t i = 0; i < BTN_COUNT; i++) {
                if (BTN_EDGE(i)) {
                    outputs_reset();
                    car_reset(0); car_reset(1);
                    g_phase = PHASE_INTRO;
                    scr_intro();
                    break;
                }
            }
            break;
        }
    }

    return 0;
}
