#include <avr/io.h>
#include <avr/interrupt.h>

ISR(TIMER1_CAPT_vect)
{
    // Button currently HIGH = pressed
    if (PINB & (1 << PB0))
    {
        PORTB |= (1 << PB5);       // LED ON
        // Detect falling edge next
        TCCR1B &= ~(1 << ICES1);
    }
    else
    {
        PORTB &= ~(1 << PB5);      // LED OFF
        // Detect rising edge next
        TCCR1B |= (1 << ICES1);
    }
    // Clear input capture flag
    TIFR1 = (1 << ICF1);
}

int main(void)
{
    // PB0 input
    DDRB &= ~(1 << PB0);
    PORTB &= ~(1 << PB0);

    // PB5 onboard LED output
    DDRB |= (1 << PB5);
    PORTB &= ~(1 << PB5);

    // Reset Timer1 configuration
    TCCR1A = 0;
    TCCR1B = 0;

    // Rising edge first
    TCCR1B |= (1 << ICES1);

    // Start Timer1: clock / 64
    TCCR1B |= (1 << CS11) | (1 << CS10);

    // Clear pending capture flag
    TIFR1 = (1 << ICF1);

    // Enable Timer1 Input Capture interrupt
    TIMSK1 |= (1 << ICIE1);

    // Global interrupt enable
    sei();

    while (1)
    {
        // Everything happens in ISR
    }
}

