#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include "uart.h"

#define TX_REQ PD2      // INT0
#define RX_REQ PD3      // INT1
#define TX_ACK PD4      // Output
#define RX_NEW_DATA PD5 // Output

#define BUFFER_LEN 1024
volatile char rx_buffer[BUFFER_LEN];
volatile int head = 0; 
volatile int tail = 0; 

void interrupt_init(void) 
{
    DDRD &= ~((1 << TX_REQ) | (1 << RX_REQ)); // Set as input
    PORTD |= (1 << TX_REQ) | (1 << RX_REQ);   // Enable pull-ups
    
    MCUCR |= (1 << ISC11) | (1 << ISC01);     // Set falling edge trigger
    MCUCR &= ~((1 << ISC10) | (1 << ISC00));
    
    GICR |= (1 << INT0) | (1 << INT1);        // Enable external interrupts
    sei();                                    // Enable global interrupts
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

// 6502 to ATmega transmission
ISR(INT0_vect) 
{
    uint8_t data = PINA;             // Read from data bus
    UART_transmit(data);
    
    PORTD &= ~(1 << TX_ACK);         // Set TX ACK low (Data received)
    while (!(PIND & (1 << TX_REQ))); // Wait until 6502 releases TX REQ
    PORTD |= (1 << TX_ACK);          // Set TX ACK high (Ready)
}

// ATmega to 6502 transmission
ISR(INT1_vect)  
{
    if (head != tail) 
    {
        DDRA = 0xFF;                 // Set data bus as output
        PORTA = rx_buffer[tail];     // Put data on bus
        tail = (tail + 1) % BUFFER_LEN;
        
        PORTD |= (1 << RX_NEW_DATA); // Set high (Data is ready on bus)
        while (!(PIND & (1 << RX_REQ))); // Wait until 6502 releases RX REQ
        
        DDRA = 0x00;                 // Set data bus back to input
        PORTA = 0x00;
    }
}

int main(void) 
{
    DDRA = 0x00;                     // Set data bus as input initially
    PORTA = 0x00;

    DDRD |= (1 << TX_ACK) | (1 << RX_NEW_DATA); // Set control pins as output
    PORTD |= (1 << TX_ACK);                     // Default HIGH
    PORTD |= (1 << RX_NEW_DATA);                // Default HIGH (No data)

    UART_init(9600);
    interrupt_init();

    while (1) 
    {
        if (head != tail) 
        {
            PORTD &= ~(1 << RX_NEW_DATA); // Data available (Active-Low)
        }
        else 
        {
            PORTD |= (1 << RX_NEW_DATA);  // Buffer empty
        }
    }
    return 0;
}