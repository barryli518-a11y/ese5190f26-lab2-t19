#ifndef F_CPU
#define F_CPU 16000000UL
#endif

#include <avr/io.h>
#include "UART.h"

#define BAUD_RATE 9600UL
#define UBRR_VALUE ((F_CPU / (16UL * BAUD_RATE)) - 1UL)

void uart_init(void)
{
    UBRR0H = (unsigned char)(UBRR_VALUE >> 8);
    UBRR0L = (unsigned char)UBRR_VALUE;
    UCSR0A = 0;
    UCSR0B = (1 << TXEN0);
    UCSR0C = (1 << UCSZ01) | (1 << UCSZ00); // 8-N-1
}

void uart_putc(char character)
{
    while (!(UCSR0A & (1 << UDRE0))) {
    }
    UDR0 = character;
}

void uart_puts(const char *text)
{
    while (*text != '\0') {
        uart_putc(*text++);
    }
}
