#ifndef __I2C_H
#define __I2C_H
#include "sys.h"
#include "gpio.h"

#define I2C_SOFE  1  //I2C软件操控

typedef struct {
  I2C_TypeDef* the;
  uint32_t RCC_The;
  GpioStruct *SCL;
  GpioStruct *SDA;
  uint8_t  IRQn;
} I2cStruct;  //I2c结构

I2cStruct* i2c_new(uint8_t t);
void i2c_setGPIO(I2cStruct* self, GpioStruct *P_SCL, GpioStruct *P_SDA);
void i2c_init(I2cStruct* self);
void i2c_start(I2cStruct* self);
void i2c_stop(I2cStruct* self);
void i2c_ack(I2cStruct* self, uint8_t en);
ErrorStatus i2c_waitAck(I2cStruct* self);
ErrorStatus i2c_write(I2cStruct* self, uint8_t data);
uint8_t i2c_read(I2cStruct* self, uint8_t isAck);

#endif //__I2C_H
