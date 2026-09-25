#ifndef F_CPU
#define F_CPU 16000000UL
#endif

#include <avr/io.h>
#include <util/delay.h>
#include <stdint.h>

// Part A, S1: all four LEDs blink together every 500 ms.
#define LED_MASK ((1U << PB1) | (1U << PB2) | (1U << PB3) | (1U << PB4))

int main(void)
{
    DDRB |= LED_MASK;

    for (;;) {
        PORTB |= LED_MASK;
        _delay_ms(500);
        PORTB &= (uint8_t)~LED_MASK;
        _delay_ms(500);
    }
}
