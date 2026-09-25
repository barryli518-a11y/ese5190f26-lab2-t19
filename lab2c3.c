
#define F_CPU 16000000UL
#include <avr/io.h>
#include <util/delay.h>
#include <stdio.h>
#include "uart.h"


#define DOT_LED  PB1
#define DASH_LED PB2

// Standard Part C durations: Dot = 150 ms, Dash = 300 ms
#define DOT_TIME_MS       150
#define DASH_TIME_MS      300
#define SYMBOL_SPACE_MS   150   // Pause between dots/dashes within a letter
#define LETTER_SPACE_MS   450   // Pause between letters (>400 ms)
#define WORD_SPACE_MS    1000   // Pause between words

// Morse lookup table for letters A-Z
const char *morse_table[26] = {
    ".-",    // A
    "-...",  // B
    "-.-.",  // C
    "-..",   // D
    ".",     // E
    "..-.",  // F
    "--.",   // G
    "....",  // H
    "..",    // I
    ".---",  // J
    "-.-",   // K
    ".-..",  // L
    "--",    // M
    "-.",    // N
    "---",   // O
    ".--.",  // P
    "--.-",  // Q
    ".-.",   // R
    "...",   // S
    "-",     // T
    "..-",   // U
    "...-",  // V
    ".--",   // W
    "-..-",  // X
    "-.--",  // Y
    "--.."   // Z
};

void flash_dot(void)
{
    PORTB |= (1 << DOT_LED);      // Dot LED ON
    _delay_ms(DOT_TIME_MS);
    PORTB &= ~(1 << DOT_LED);     // Dot LED OFF
    _delay_ms(SYMBOL_SPACE_MS);
}

void flash_dash(void)
{
    PORTB |= (1 << DASH_LED);     // Dash LED ON
    _delay_ms(DASH_TIME_MS);
    PORTB &= ~(1 << DASH_LED);    // Dash LED OFF
    _delay_ms(SYMBOL_SPACE_MS);
}

void send_morse_char(const char *code)
{
    for (int i = 0; code[i] != '\0'; i++)
    {
        if (code[i] == '.')
        {
            flash_dot();
        }
        else if (code[i] == '-')
        {
            flash_dash();
        }
    }
    // Extra pause between letters
    _delay_ms(LETTER_SPACE_MS - SYMBOL_SPACE_MS);
}

int main(void)
{
    // Configure PB1 and PB2 as outputs for the LEDs
    DDRB |= (1 << DOT_LED) | (1 << DASH_LED);

    // Start with LEDs OFF
    PORTB &= ~((1 << DOT_LED) | (1 << DASH_LED));

    // Hardcoded message below 
    const char message[] = "COPYRIGHTLABEL";
    uart_init();
    while (1)
    {
        for (int i = 0; message[i] != '\0'; i++)
        {
            char c = message[i];
	    printf("%c\n", c);

            if (c >= 'A' && c <= 'Z')
            {
                // Convert uppercase ASCII letter to 0-25 array index
                send_morse_char(morse_table[c - 'A']);
            }
            else if (c >= 'a' && c <= 'z')
            {
                // Convert lowercase ASCII letter to 0-25 array index
                send_morse_char(morse_table[c - 'a']);
            }
            else if (c == ' ')
            {
                _delay_ms(WORD_SPACE_MS);
            }
        }

        // 3-second pause before repeating the entire message loop
        _delay_ms(3000);
    }

    return 0;
}

