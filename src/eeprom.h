#ifndef EEPROM_H_
#define EEPROM_H_

void writeEEPROM(unsigned int memory_address, unsigned char data);
void setEEPROMpointer(unsigned int memory_address);
void readEEPROM();
void setEEPROM(void);
unsigned char setI2C_ADDR();
uint8_t setEEPROMBusAddr();
#endif