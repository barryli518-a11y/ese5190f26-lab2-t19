#ifndef F_CPU
#define F_CPU 16000000UL
#endif

#include <avr/io.h>
#include <util/delay.h>
#include <stdint.h>

// Part A, S3: PB0 is pulled down externally and goes high when pressed.
// Each LED is wired from a PB1-PB4 output through a 1 kOhm resistor to GND.
#define BUTTON_MASK (1U << PB0)
#define LED_MASK ((1U << PB1) | (1U << PB2) | (1U << PB3) | (1U << PB4))
#define DEBOUNCE_MS 20U

static void show_led(uint8_t index)
{
    PORTB = (PORTB & (uint8_t)~LED_MASK) | (uint8_t)(1U << (PB1 + index));
}

int main(void)
{
    uint8_t led_index = 0;
    uint8_t last_sample;
    uint8_t stable_state;
    uint8_t matching_ms = 0;

    DDRB &= (uint8_t)~BUTTON_MASK;  // PB0 input
    PORTB &= (uint8_t)~BUTTON_MASK; // No internal pull-up; use 10 kOhm pull-down
    DDRB |= LED_MASK;               // PB1-PB4 outputs
    show_led(led_index);            // Initially only PB1 is on

    last_sample = (PINB & BUTTON_MASK) != 0;
    stable_state = last_sample;

    for (;;) {
        uint8_t sample = (PINB & BUTTON_MASK) != 0;

        if (sample != last_sample) {
            last_sample = sample;
            matching_ms = 0;
        } else if (matching_ms < DEBOUNCE_MS) {
            matching_ms++;
        }

        if (matching_ms == DEBOUNCE_MS && sample != stable_state) {
            stable_state = sample;
            if (stable_state) {
                led_index = (led_index + 1U) & 3U;
                show_led(led_index);
            }
        }

        _delay_ms(1);
    }
}
