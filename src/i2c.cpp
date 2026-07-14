#include <avr/io.h>
#include <avr/delay.h>
#include "i2c.h"
#ifndef F_CPU
#define F_CPU 16000000UL 
#endif

#define SCL_CLK 100000UL  // 100 kHz
#define I2C_PRESCALER 1
#define TWBR_VALUE (((F_CPU / SCL_CLK) - 16) / (2 * I2C_PRESCALER))

#define I2C_TIMEOUT_LIMIT  20000

#define I2C_OK             0
#define I2C_ERROR_TIMEOUT  1
#define I2C_ERROR_NACK     2

void I2C_Init(void) {
    TWBR = TWBR_VALUE; 
    TWSR &= ~((1 << TWPS1) | (1 << TWPS0));
    TWCR = (1 << TWEN);
}

void I2C_Start(void) {
    TWCR = (1<<TWINT) | (1<<TWSTA) | (1<<TWEN);
    while (!(TWCR & (1<<TWINT)));
}

void I2C_Write(unsigned char data) {
    TWDR = data;
    TWCR = (1<<TWINT) | (1<<TWEN);
    while (!(TWCR & (1<<TWINT)));
}

uint8_t I2C_Write_Timeout(unsigned char data) {
    uint16_t timeout = I2C_TIMEOUT_LIMIT;
    
    TWDR = data;
    TWCR = (1 << TWINT) | (1 << TWEN);
    
    while (!(TWCR & (1 << TWINT))) {
        if (--timeout == 0) {
            return I2C_ERROR_TIMEOUT; 
        }
    }
    
    unsigned char status = TWSR & 0xF8;
    if (status != 0x18 && status != 0x28 && status != 0x40) {
        return I2C_ERROR_NACK;
    }
    
    return I2C_OK; 
}

void I2C_Stop(void) {
    TWCR = (1 << TWINT) | (1 << TWSTO) | (1 << TWEN);
    _delay_us(10); 
}

unsigned char I2C_Read_ACK(void) {
    TWCR = (1 << TWINT) | (1 << TWEN) | (1 << TWEA);
    while (!(TWCR & (1 << TWINT)));
    return TWDR;
}

unsigned char I2C_Read_NACK(void) {
    TWCR = (1 << TWINT) | (1 << TWEN);
    while (!(TWCR & (1 << TWINT)));
    return TWDR;
}