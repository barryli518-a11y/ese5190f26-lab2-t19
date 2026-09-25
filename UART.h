#ifndef UART_H
#define UART_H

void uart_init(void);
void uart_putc(char character);
void uart_puts(const char *text);

#endif
