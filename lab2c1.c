#define F_CPU 16000000UL

#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdio.h>
#include "uart.h"

// Timer1 uses:
// 16 MHz / 256 = 62,500 Hz
// 1 tick = 16 us

#define DOT_MIN_TICKS      3125U    // 50 ms
#define DOT_MAX_TICKS     12500U    // 200 ms
#define DASH_MAX_TICKS    25000U    // 400 ms
#define SPACE_TICKS       25000U    // 400 ms

volatile uint16_t press_start = 0;
volatile uint16_t press_duration = 0;
volatile uint8_t button_event = 0;
volatile uint8_t button_pressed = 0;

ISR(TIMER1_CAPT_vect)
{
    // PB0 HIGH means button is pressed
    if (PINB & (1 << PB0))
    {
        button_pressed = 1;

        // Record when the button was pressed
        press_start = TCNT1;

        // Optional: onboard LED ON while button is held
        PORTB |= (1 << PB5);

        // Next interrupt should detect falling edge
        TCCR1B &= ~(1 << ICES1);
    }
    else
    {
        button_pressed = 0;

        // Measure how long button was held
        press_duration = TCNT1 - press_start;

        // Tell main() that a complete press/release happened
        button_event = 1;

        // Onboard LED OFF
        PORTB &= ~(1 << PB5);

        // Next interrupt should detect rising edge
        TCCR1B |= (1 << ICES1);
    }

    // Clear input capture flag
    TIFR1 = (1 << ICF1);
}

int main(void)
{
    // -------------------------
    // GPIO
    // -------------------------

    // PB0 = button input
    DDRB &= ~(1 << PB0);

    // External 10k pull-down is used
    PORTB &= ~(1 << PB0);

    // PB5 = onboard LED output
    DDRB |= (1 << PB5);
    PORTB &= ~(1 << PB5);


    // -------------------------
    // UART
    // -------------------------

    uart_init();

    printf("\r\nPart C started\r\n");
    printf("Waiting for Morse input...\r\n");


    // -------------------------
    // Timer1
    // -------------------------

    TCCR1A = 0;
    TCCR1B = 0;

    // Reset counter
    TCNT1 = 0;

    // ICNC1 = input capture noise canceler
    // ICES1 = initially detect rising edge
    // CS12  = prescaler /256
    TCCR1B =
          (1 << ICNC1)
        | (1 << ICES1)
        | (1 << CS12);

    // Clear old capture flag
    TIFR1 = (1 << ICF1);

    // Enable Timer1 Input Capture interrupt
    TIMSK1 |= (1 << ICIE1);

    // Enable global interrupts
    sei();


    uint8_t space_reported = 1;
    uint16_t release_time = 0;

    while (1)
    {
        // -----------------------------------
        // A complete button press happened
        // -----------------------------------
        if (button_event)
        {
            uint16_t duration;

            // Briefly disable interrupts while copying shared variable
            cli();
            duration = press_duration;
            button_event = 0;
            sei();

            // Remember when button was released
            release_time = TCNT1;
            space_reported = 0;

            printf("Duration: %u ticks | ", duration);

            if (duration < DOT_MIN_TICKS)
            {
                // Usually bounce / accidental very short press
                printf("ignored (too short)\r\n");
            }
            else if (duration < DOT_MAX_TICKS)
            {
                printf("DOT .\r\n");
            }
            else if (duration <= DASH_MAX_TICKS)
            {
                printf("DASH -\r\n");
            }
            else
            {
                printf("ignored (too long)\r\n");
            }
        }


        // -----------------------------------
        // Detect >400 ms gap = SPACE
        // -----------------------------------
        if (!button_pressed && !space_reported)
        {
            uint16_t idle_time = TCNT1 - release_time;

            if (idle_time >= SPACE_TICKS)
            {
                printf("SPACE\r\n");
                space_reported = 1;
            }
        }
    }
}

