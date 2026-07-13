#ifndef UART_H_
#define UART_H_

#include "pins.h"
#include <avr/io.h>

#define BUFFER_LEN 1024

extern volatile char rx_buffer[BUFFER_LEN];
extern volatile int head;
extern volatile int tail;
extern volatile uint8_t last_command;

void UART_init(uint32_t baudrate);
void UART_transmit(char data);
char UART_receive(void);
void UART_print(const char* str);

#endif 