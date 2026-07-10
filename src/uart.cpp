#include "uart.h"
#ifndef F_CPU
#define F_CPU 16000000UL 
#endif

void UART_init(uint32_t baudrate) {
    uint16_t ubrr_value = (F_CPU / (16UL * baudrate)) - 1;
    UBRRH = (uint8_t)(ubrr_value >> 8);
    UBRRL = (uint8_t)ubrr_value;
    UCSRB = (1 << RXEN) | (1 << TXEN) | (1 << RXCIE);    
    UCSRC = (1 << URSEL) | (1 << UCSZ1) | (1 << UCSZ0);
}

void UART_transmit(char data) {
    while (!(UCSRA & (1 << UDRE)));
    UDR = data;
}

char UART_receive(void) {
    while (!(UCSRA & (1 << RXC)));
}