#ifndef I2C_H_
#define I2C_H_

void I2C_Init();
void I2C_Start();
void I2C_Write(unsigned char data);
void I2C_Stop();
unsigned char I2C_Read_ACK();
unsigned char I2C_Read_NACK();

#endif 