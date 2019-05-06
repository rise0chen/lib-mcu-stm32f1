#ifndef __I2C_H
#define __I2C_H
#include "sys.hpp"
#include "gpio.hpp"

#define I2C_SOFE  1  //I2C软件操控

class I2c{
	public:
		//BufRcv tx;
		//BufRcv rx;
		
		I2c(uint8_t t);
		void setGPIO(Gpio *P_SCL, Gpio *P_SDA);
		void init(void);
		void start(void);
		void stop(void);
		void ack(uint8_t en=1);
		ErrorStatus waitAck(void);
		ErrorStatus write(uint8_t data);
		uint8_t read(uint8_t isAck);
	
	private:
		I2C_TypeDef* the;
		uint32_t RCC_The;
		Gpio *SCL;
		Gpio *SDA;
		uint8_t  IRQn;
};
extern I2c i2c1;
extern I2c i2c2;

#endif //__I2C_H
