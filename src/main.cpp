#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/interrupt.h>
#include <avr/delay.h>
#include "uart.h"

#define CA1 PD6
#define CA2 PD5
#define BUFFER_LEN 1024

volatile char rx_buffer[BUFFER_LEN];
volatile int head = 0; 
volatile int tail = 0; 

void interrupt_init(void) 
{
    DDRD &= ~(1 << PD2);
    PORTD |= (1 << PD2);
    MCUCR |= (1 << ISC01);
    MCUCR &= ~(1 << ISC00);
    GICR |= (1 << INT0);
    sei();
}

ISR(USART_RXC_vect)
{
    char rx_data = UDR;
    int next_head = (head + 1) % BUFFER_LEN; 
    if (next_head != tail) 
    {
        rx_buffer[head] = rx_data;
        head = next_head;
    }
}

ISR(INT0_vect) 
{
    uint8_t data = PINA;
    PORTD &= ~(1 << PD7);
    _delay_us(2); // Kısa bir ACK
    PORTD |= (1 << PD7);
    UART_transmit(data);
}


int main(void) 
{
    DDRA = 0x00; 
    DDRC = 0xFF; 
    PORTA = 0x00;
    PORTC = 0x00; 
    
    DDRD |= (1 << PD7); 

    DDRD |= (1 << CA1); 
    DDRD &= ~(1 << CA2); 

    PORTD |= (1 << CA1);
    PORTD &= ~(1 << CA2);
    PORTD |= (1 << PD7);

    UART_init(9600);
    interrupt_init();

    while (1) 
    {
        if (head != tail)
        {
            PORTC = rx_buffer[tail];
            _delay_us(1);
            PORTD &= ~(1 << CA1);
            _delay_us(2);  
            PORTD |= (1 << CA1);
            while (!(PIND & (1 << CA2)));
            tail = (tail + 1 ) % BUFFER_LEN;
        }
    }
    
    return 0;
}

