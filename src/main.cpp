#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include "pins.h"
#include "uart.h"
#include "i2c.h"
#include "eeprom.h"

volatile char rx_buffer[BUFFER_LEN_RX];
volatile int rx_head = 0;
volatile int rx_tail = 0;

volatile char tx_buffer[BUFFER_LEN_TX];
volatile int tx_head = 0;
volatile int tx_tail = 0;

volatile int eeprom_loader = 0;
volatile uint8_t command;


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
    int next_head = (rx_head + 1) % BUFFER_LEN_RX; 
    if (next_head != rx_tail) 
    {
        rx_buffer[rx_head] = rx_data;
        rx_head = next_head;
    }
}

// 6502 to ATmega transmission
ISR(INT0_vect) 
{
    uint8_t data = PINA;             // Read from data bus
    uint8_t command_mode = PIND & (1 << COMMAND);
    if (!command_mode){
        UART_transmit(data);
    }
    else {
        int next_head = (tx_head + 1) % BUFFER_LEN_TX; 
        if (next_head != tx_tail) 
        {
           
            tx_buffer[tx_head] = data;
            tx_head = next_head;
        }
    }

    PORTD &= ~(1 << TX_ACK);         // Set TX ACK low (Data received)
    while (!(PIND & (1 << TX_REQ))); // Wait until 6502 releases TX REQ
    PORTD |= (1 << TX_ACK);          // Set TX ACK high (Ready)
}

// ATmega to 6502 transmission
ISR(INT1_vect)  
{
    if (rx_head != rx_tail) 
    {
        DDRA = 0xFF;                 // Set data bus as output
        PORTA = rx_buffer[rx_tail];     // Put data on bus
        rx_tail = (rx_tail + 1) % BUFFER_LEN_RX;
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

    DDRD |= (1 << TX_ACK) | (1 << RX_NEW_DATA) | (0 << COMMAND); // Set control pins
    PORTD |= (1 << TX_ACK);                     // Default HIGH
    PORTD |= (1 << RX_NEW_DATA);                // Default HIGH (No data)
    PORTD |= (1 << COMMAND);                // Default HIGH (No data)

    UART_init(9600);
    I2C_Init();
    interrupt_init();
    
    while (1) 
    {
        command = readData();
        
        switch (command)
        {
            case 0x01:
                    _delay_ms(10);
                    UART_writeline("SEARCHING... ");
                    setEEPROMpointer(0);
                    readEEPROM();
                    
                break;
            case 0x02:
                setEEPROM();
                break;
            default:
                break;
        }

        if (rx_head != rx_tail) PORTD &= ~(1 << RX_NEW_DATA); // Data available (Active-Low)        
        else  PORTD |= (1 << RX_NEW_DATA);  // Buffer empty
    }
    
    return 0;
}