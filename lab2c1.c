#include <avr/io.h>
#include <avr/interrupt.h>

// Part B: external button on PB0/ICP1, with a 10 kOhm pull-down.
// The Xplained Mini's built-in LED is on PB5 (high = on).
ISR(TIMER1_CAPT_vect)
{
    if (PINB & (1 << PB0)) {
        PORTB |= (1 << PB5);       // Pressed: LED on
        TCCR1B &= ~(1 << ICES1);   // Capture the falling edge next
    } else {
        PORTB &= ~(1 << PB5);      // Released: LED off
        TCCR1B |= (1 << ICES1);    // Capture the rising edge next
    }

    TIFR1 = (1 << ICF1);           // Clear capture flag after changing edge
}

int main(void)
{
    DDRB &= ~(1 << PB0);           // PB0 input
    PORTB &= ~(1 << PB0);          // External pull-down; no internal pull-up
    DDRB |= (1 << PB5);            // Built-in LED output

    TCCR1A = 0;                   // Timer1 normal mode

    if (PINB & (1 << PB0)) {
        PORTB |= (1 << PB5);
        TCCR1B = (1 << ICNC1) | (1 << CS11) | (1 << CS10);
    } else {
        PORTB &= ~(1 << PB5);
        TCCR1B = (1 << ICNC1) | (1 << ICES1) | (1 << CS11) | (1 << CS10);
    }

    TIFR1 = (1 << ICF1);           // Discard any capture during setup
    TIMSK1 |= (1 << ICIE1);        // Enable Timer1 input capture interrupt
    sei();                         // Enable global interrupts

    while (1) {
        // The interrupt handles button changes.
    }
}
