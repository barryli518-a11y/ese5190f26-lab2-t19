#include <avr/io.h>

// PB0: button to 5 V with a 10 kOhm pull-down to GND.
// PB1: LED and 1 kOhm resistor to GND.

int main(void)
{
    DDRB &= ~(1 << PB0);  // Button input
    PORTB &= ~(1 << PB0); // Disable internal pull-up
    DDRB |= (1 << PB1);   // LED output

    while (1) {
        if (PINB & (1 << PB0)) {
            PORTB |= (1 << PB1);  // Pressed: LED on
        } else {
            PORTB &= ~(1 << PB1); // Released: LED off
        }
    }
}
