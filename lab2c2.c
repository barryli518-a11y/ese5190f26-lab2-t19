#define F_CPU 16000000UL

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <stdio.h>
#include <string.h>
#include "uart.h"

#define DOT_MIN_TICKS      3125U    // 50 ms
#define DOT_MAX_TICKS     12500U    // 200 ms
#define DASH_MAX_TICKS    25000U    // 400 ms
#define SPACE_TICKS       25000U    // 400 ms

#define DOT_LED   PB1
#define DASH_LED  PB2

// -------------------------------------------------
// Shared variables between ISR and main()
// -------------------------------------------------

volatile uint16_t press_start = 0;
volatile uint16_t press_duration = 0;
volatile uint8_t button_event = 0;
volatile uint8_t button_pressed = 0;

char morse_buffer[6];
uint8_t morse_index = 0;


char decode_morse(const char *code)
{
    // Letters A-Z
    static const char *letters[] =
    {
        ".-",     // A
        "-...",   // B
        "-.-.",   // C
        "-..",    // D
        ".",      // E
        "..-.",   // F
        "--.",    // G
        "....",   // H
        "..",     // I
        ".---",   // J
        "-.-",    // K
        ".-..",   // L
        "--",     // M
        "-.",     // N
        "---",    // O
        ".--.",   // P
        "--.-",   // Q
        ".-.",    // R
        "...",    // S
        "-",      // T
        "..-",    // U
        "...-",   // V
        ".--",    // W
        "-..-",   // X
        "-.--",   // Y
        "--.."    // Z
    };

    // Digits 0-9
    static const char *digits[] =
    {
        "-----",  // 0
        ".----",  // 1
        "..---",  // 2
        "...--",  // 3
        "....-",  // 4
        ".....",  // 5
        "-....",  // 6
        "--...",  // 7
        "---..",  // 8
        "----."   // 9
    };

    // Search letters
    for (uint8_t i = 0; i < 26; i++)
    {
        if (strcmp(code, letters[i]) == 0)
        {
            return 'A' + i;
        }
    }

    // Search digits
    for (uint8_t i = 0; i < 10; i++)
    {
        if (strcmp(code, digits[i]) == 0)
        {
            return '0' + i;
        }
    }

    // Unknown Morse sequence
    return '?';
}



void add_morse_symbol(char symbol)
{
    if (morse_index < 5)
    {
        morse_buffer[morse_index] = symbol;
        morse_index++;

        // Always keep it null terminated
        morse_buffer[morse_index] = '\0';
    }
}


void flash_dot_led(void)
{
    PORTB |= (1 << DOT_LED);

    _delay_ms(50);

    PORTB &= ~(1 << DOT_LED);
}


void flash_dash_led(void)
{
    PORTB |= (1 << DASH_LED);

    _delay_ms(50);

    PORTB &= ~(1 << DASH_LED);
}


ISR(TIMER1_CAPT_vect)
{
    // PB0 HIGH = button pressed
    if (PINB & (1 << PB0))
    {
        button_pressed = 1;

        // Save press start time
        press_start = TCNT1;

        // Next interrupt = falling edge
        TCCR1B &= ~(1 << ICES1);
    }
    else
    {
        button_pressed = 0;

        // How long was button held?
        press_duration = TCNT1 - press_start;

        // Tell main() a complete press/release happened
        button_event = 1;

        // Next interrupt = rising edge
        TCCR1B |= (1 << ICES1);
    }

    // Clear input capture flag
    TIFR1 = (1 << ICF1);
}


int main(void)
{
    // PB0 = button input
    DDRB &= ~(1 << PB0);

    // External 10k pull-down
    PORTB &= ~(1 << PB0);

    // PB1 = DOT LED output
    DDRB |= (1 << DOT_LED);

    // PB2 = DASH LED output
    DDRB |= (1 << DASH_LED);

    // Start both LEDs OFF
    PORTB &= ~(1 << DOT_LED);
    PORTB &= ~(1 << DASH_LED);

    uart_init();

    printf("\r\n");
    printf("Part E Morse Decoder Started\r\n");
    printf("----------------------------\r\n");
    printf("Enter Morse using the button.\r\n");

    TCCR1A = 0;
    TCCR1B = 0;

    TCNT1 = 0;

    // ICNC1 = noise canceler
    // ICES1 = start by detecting rising edge
    // CS12  = prescaler /256
    TCCR1B =
          (1 << ICNC1)
        | (1 << ICES1)
        | (1 << CS12);

    // Clear old capture flag
    TIFR1 = (1 << ICF1);

    // Enable Input Capture interrupt
    TIMSK1 |= (1 << ICIE1);

    // Global interrupts ON
    sei();

    uint16_t release_time = 0;

    // Prevent printing a character repeatedly
    uint8_t character_finished = 1;

    morse_buffer[0] = '\0';

    while (1)
    {

        if (button_event)
        {
            uint16_t duration;

            // press_duration is 16 bit,
            // so safely copy it while interrupts are off
            cli();

            duration = press_duration;
            button_event = 0;

            sei();

            // Remember when release occurred
            release_time = TCNT1;

            // Current Morse character is still being built
            character_finished = 0;


            if (duration < DOT_MIN_TICKS)
            {
                printf("Ignored: too short\r\n");
            }

            else if (duration < DOT_MAX_TICKS)
            {
                printf("DOT .\r\n");
                add_morse_symbol('.');
                flash_dot_led();
            }

            else if (duration <= DASH_MAX_TICKS)
            {
                printf("DASH -\r\n");
                add_morse_symbol('-');
                flash_dash_led();
            }
              
            else
            {
                printf("Ignored: press too long\r\n");
            }
        }


        // >400 ms with no press:
        // end of one Morse CHARACTER

        if (!button_pressed && !character_finished)
        {
            uint16_t idle_time = TCNT1 - release_time;
            if (idle_time >= SPACE_TICKS)
            {
                character_finished = 1;
                // Only decode if we actually have symbols
                if (morse_index > 0)
                {
                    char decoded;
                    decoded = decode_morse(morse_buffer);
                    printf(
                        "Morse: %s  ->  Character: %c\r\n",
                        morse_buffer,
                        decoded
                    );
                    morse_index = 0;
                    morse_buffer[0] = '\0';
                }
            }
        }
    }
}

