#ifndef __I2C_H
#define __I2C_H
#include "sys.hpp"
#include "gpio.hpp"

#define I2C_SOFE  1  //I2C软件操控

class I2c{
	public:
		//BufRcv tx;
		//BufRcv rx;
		
		I2c(u8 t);
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
		Gpio SCL;
		Gpio SDA;
		u8  IRQn;
};
extern I2c i2c1;
extern I2c i2c2;

#endif //__I2C_H
