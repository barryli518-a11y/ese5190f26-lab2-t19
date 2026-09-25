#ifndef F_CPU
#define F_CPU 16000000UL
#endif

#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdint.h>
#include "UART.h"

// Timer1 uses a /64 prescaler: one timer tick = 4 microseconds.
#define DOT_MIN_TICKS    12500UL  // 50 ms
#define DASH_MIN_TICKS   50000UL  // 200 ms
#define DASH_MAX_TICKS  100000UL  // 400 ms
#define SPACE_TICKS     100000UL  // 400 ms released

static volatile uint16_t timer_overflows;
static volatile uint32_t press_time;
static volatile uint32_t release_time;
static volatile char symbol_ready;
static volatile uint8_t button_down;
static volatile uint8_t space_sent = 1;

static uint32_t captured_time(void)
{
    uint16_t upper = timer_overflows;
    uint16_t lower = ICR1;

    // Include an overflow that occurred before capture but whose ISR is pending.
    if ((TIFR1 & (1 << TOV1)) && lower < 0x8000) {
        upper++;
    }

    return ((uint32_t)upper << 16) | lower;
}

static uint32_t current_time(void)
{
    uint8_t old_sreg = SREG;
    uint16_t upper;
    uint16_t lower;

    cli();
    upper = timer_overflows;
    lower = TCNT1;
    if ((TIFR1 & (1 << TOV1)) && lower < 0x8000) {
        upper++;
    }
    SREG = old_sreg;

    return ((uint32_t)upper << 16) | lower;
}

ISR(TIMER1_OVF_vect)
{
    timer_overflows++;
}

ISR(TIMER1_CAPT_vect)
{
    uint32_t now = captured_time();

    if (PINB & (1 << PB0)) {
        // Rising edge: button pressed.
        press_time = now;
        button_down = 1;
        PORTB |= (1 << PB5);
        TCCR1B &= ~(1 << ICES1);   // Capture falling edge next
    } else {
        // Falling edge: button released.
        uint32_t held_ticks = now - press_time;

        button_down = 0;
        release_time = now;
        PORTB &= ~(1 << PB5);
        TCCR1B |= (1 << ICES1);    // Capture rising edge next

        // Ignore transitions shorter than 50 ms as switch bounce.
        if (held_ticks >= DOT_MIN_TICKS && held_ticks < DASH_MIN_TICKS) {
            symbol_ready = '.';
            space_sent = 0;
        } else if (held_ticks >= DASH_MIN_TICKS &&
                   held_ticks <= DASH_MAX_TICKS) {
            symbol_ready = '-';
            space_sent = 0;
        }
    }

    TIFR1 = (1 << ICF1);           // Clear flag after changing capture edge
}

int main(void)
{
    uart_init();

    DDRB &= ~(1 << PB0);           // PB0/ICP1 input
    PORTB &= ~(1 << PB0);          // External 10 kOhm pull-down
    DDRB |= (1 << PB5);            // Built-in LED output
    PORTB &= ~(1 << PB5);

    TCCR1A = 0;                    // Timer1 normal mode
    TCCR1B = (1 << ICNC1) |        // Input capture noise canceler
             (1 << ICES1) |        // Begin with rising-edge capture
             (1 << CS11) | (1 << CS10); // Clock / 64
    TCNT1 = 0;
    TIFR1 = (1 << ICF1) | (1 << TOV1);
    TIMSK1 = (1 << ICIE1) | (1 << TOIE1);
    sei();

    uart_puts("Part C ready: . = dot, - = dash, space = gap\r\n");

    while (1) {
        char symbol;
        uint8_t down;
        uint8_t gap_already_sent;
        uint32_t last_release;
        uint8_t old_sreg = SREG;

        cli();
        symbol = symbol_ready;
        symbol_ready = 0;
        down = button_down;
        gap_already_sent = space_sent;
        last_release = release_time;
        SREG = old_sreg;

        if (symbol != 0) {
            uart_putc(symbol);
        }

        if (!down && !gap_already_sent &&
            (current_time() - last_release) > SPACE_TICKS) {
            old_sreg = SREG;
            cli();
            if (!button_down && !space_sent) {
                space_sent = 1;
                SREG = old_sreg;
                uart_putc(' ');
            } else {
                SREG = old_sreg;
            }
        }
    }
}
