#ifndef __I2C_H
#define __I2C_H
#include "sys.hpp"
#include "gpio.hpp"

#define I2C_SOFE  1  //I2C软件操控

class i2c{
	public:
		//char TX_BUF[LEN_MID];
		//char RX_BUF[LEN_MID];
		
		i2c(u8 t);
		void Config(void);
		void Start(void);
		void Stop(void);
		void Ack(u8 en=1);
		ErrorStatus WaitAck(void);
		ErrorStatus Write(u8 data);
		u8 Read(u8 ack);
	
	private:
		I2C_TypeDef* the;
		u16 RCC_GPIO;
		u32 RCC_The;
		u8  Px;
		u8  PSCL;
		u8  PSDA;
		u8  IRQn;
};
extern i2c i2c1;
extern i2c i2c2;

#endif //__I2C_H
