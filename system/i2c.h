#ifndef __I2C_H
#define __I2C_H
#include "sys.h"
#include "gpio.h"

#define I2C_SOFE  1  //I2C软件操控

typedef struct {
  I2C_TypeDef* the;
  u32 RCC_The;
  GpioStruct *SCL;
  GpioStruct *SDA;
  u8  IRQn;
} I2cStruct;  //I2c结构

I2cStruct* i2c_new(u8 t);
void i2c_setGPIO(I2cStruct* self, GpioStruct *P_SCL, GpioStruct *P_SDA);
void i2c_init(I2cStruct* self);
void i2c_start(I2cStruct* self);
void i2c_stop(I2cStruct* self);
void i2c_ack(I2cStruct* self, u8 en);
ErrorStatus i2c_waitAck(I2cStruct* self);
ErrorStatus i2c_write(I2cStruct* self, u8 data);
u8 i2c_read(I2cStruct* self, u8 isAck);

#endif //__I2C_H
