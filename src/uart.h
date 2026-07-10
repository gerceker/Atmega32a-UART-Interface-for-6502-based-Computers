#ifndef UART_H_
#define UART_H_

#include <avr/io.h>

void UART_init(uint32_t baudrate);
void UART_transmit(char data);
char UART_receive(void);
void UART_print(const char* str);

#endif 