#ifndef __I2C_H
#define __I2C_H
#include "sys.h"
#include "gpio.hpp"

#define I2C_SOFE  1  //I2C软件操控

class I2c{
	public:
		I2c(u8 t);
		void setGPIO(Gpio *P_SCL, Gpio *P_SDA);
		void init(void);
		void start(void);
		void stop(void);
		void ack(u8 en=1);
		ErrorStatus waitAck(void);
		ErrorStatus write(u8 data);
		u8 read(u8 isAck);
	
	private:
		I2C_TypeDef* the;
		u32 RCC_The;
		Gpio *SCL;
		Gpio *SDA;
		u8  IRQn;
};
extern I2c i2c1;
extern I2c i2c2;

#endif //__I2C_H
