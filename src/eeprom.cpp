#include <avr/io.h>
#include <util/delay.h>
#include "eeprom.h"
#include "uart.h"
#include "i2c.h"
#include "pins.h"
#include <avr/interrupt.h>

#define DEFAULT_EEPROM_BUS_ADDRESS 0x50 
uint8_t EEPROM_BUS_ADDRESS = DEFAULT_EEPROM_BUS_ADDRESS;

void writeEEPROM(unsigned int memory_address, unsigned char data){
    I2C_Start();
    I2C_Write((EEPROM_BUS_ADDRESS << 1) | 0);
    I2C_Write((unsigned char)(memory_address >> 8));
    I2C_Write((unsigned char)(memory_address & 0xFF));
    I2C_Write(data);
    I2C_Stop();
    _delay_ms(5);
}

void setEEPROMpointer(unsigned int memory_address) {
    I2C_Start();
    I2C_Write_Timeout((EEPROM_BUS_ADDRESS << 1) | 0);
    I2C_Write((unsigned char)(memory_address >> 8));   // High Byte
    I2C_Write((unsigned char)(memory_address & 0xFF)); // Low Byte
    _delay_ms(5);
}

void readEEPROM(void){
    unsigned char data;
    rx_head = 0;
    rx_tail = 0;
    I2C_Start();                                      
    I2C_Write((EEPROM_BUS_ADDRESS << 1) | 1);
    data = I2C_Read_ACK();
    while (data != 0x04) 
    {
        uint8_t processed_data = (data == 0xA) ? 0x0D : data;
        int next_head = (rx_head + 1) % BUFFER_LEN_RX; 
        while (next_head == rx_tail);
        rx_buffer[rx_head] = processed_data;
        rx_head = next_head;
        data = I2C_Read_ACK();
        if (rx_head != rx_tail) PORTD &= ~(1 << RX_NEW_DATA); // Data available (Active-Low)
        else PORTD |= (1 << RX_NEW_DATA);  // Buffer empty
    }
    
    I2C_Read_NACK(); 
    I2C_Stop();
    return;
}


void setEEPROM(void){
    uint8_t data = 0xff;
    while (data == 0xff) data = readData();  
    EEPROM_BUS_ADDRESS = DEFAULT_EEPROM_BUS_ADDRESS + data;
    return;
}