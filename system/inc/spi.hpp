#ifndef __SPI_H
#define __SPI_H
#include "sys.hpp"
#include "gpio.hpp"

class Spi{
	public:
		//BufRcv tx;
		//BufRcv rx;
		void (*funRx)();
		
		Spi(uint8_t t);
		void init();
		void setSpeed(uint8_t div);
		uint8_t   rwByte(uint8_t data);
	
	private:
		SPI_TypeDef* the;
		uint32_t RCC_The;
		uint8_t  Px;
		uint8_t  PSCK;
		uint8_t  PMISO;
		uint8_t  PMOSI;
		uint8_t  IRQn;
		DMA_Channel_TypeDef* TX_DMA;
		DMA_Channel_TypeDef* RX_DMA;
		uint32_t FLAG_TC;
};
extern Spi spi1;
extern Spi spi2;

#endif //__SPI_H
