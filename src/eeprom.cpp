#include <avr/io.h>
#include <util/delay.h>
#include "eeprom.h"
#include "uart.h"
#include "i2c.h"
#include "pins.h"
#include <avr/interrupt.h>

#define EEPROM_BUS_ADDRESS 0x50 

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
    I2C_Write((EEPROM_BUS_ADDRESS << 1) | 0);          // Yazma Modu
    I2C_Write((unsigned char)(memory_address >> 8));   // High Byte
    I2C_Write((unsigned char)(memory_address & 0xFF)); // Low Byte
}

void readEEPROM(void){
    unsigned char data;
    I2C_Start();                                      
    I2C_Write((EEPROM_BUS_ADDRESS << 1) | 1);
    data = I2C_Read_ACK();
    head++;
    while (data != 0x04) 
    {
        uint8_t processed_data = (data == 0xA) ? 0x0D : data;
        int next_head = (head + 1) % BUFFER_LEN; 

        while (next_head == tail);
        
        rx_buffer[head] = processed_data;
        head = next_head;
        
        data = I2C_Read_ACK();
        if (head != tail) PORTD &= ~(1 << RX_NEW_DATA); // Data available (Active-Low)
        else PORTD |= (1 << RX_NEW_DATA);  // Buffer empty

    }

    I2C_Read_NACK(); 
    I2C_Stop();
    last_command = 0x00;
}