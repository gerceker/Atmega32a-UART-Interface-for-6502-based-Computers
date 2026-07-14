#ifndef UART_H_
#define UART_H_

#include "pins.h"
#include <avr/io.h>

#define BUFFER_LEN_RX 512
#define BUFFER_LEN_TX 16


extern volatile char rx_buffer[BUFFER_LEN_RX];
extern volatile int rx_head;
extern volatile int rx_tail;

extern volatile char tx_buffer[BUFFER_LEN_TX];
extern volatile int tx_head;
extern volatile int tx_tail;


void UART_init(uint32_t baudrate);
void UART_transmit(char data);
char UART_receive(void);
void UART_write(char* str);
void UART_writeline(char* str);

char readData();

#endif 