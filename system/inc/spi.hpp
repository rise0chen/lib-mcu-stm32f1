#ifndef __SPI_H
#define __SPI_H
#include "sys.hpp"
#include "gpio.hpp"

class Spi{
	public:
		//BufRcv tx;
		//BufRcv rx;
		void (*funRx)();
		
		Spi(u8 t);
		void init();
		void setSpeed(u8 div);
		u8   rwByte(u8 data);
	
	private:
		SPI_TypeDef* the;
		u32 RCC_The;
		u8  Px;
		u8  PSCK;
		u8  PMISO;
		u8  PMOSI;
		u8  IRQn;
		DMA_Channel_TypeDef* TX_DMA;
		DMA_Channel_TypeDef* RX_DMA;
		u32 FLAG_TC;
};
extern Spi spi1;
extern Spi spi2;

#endif //__SPI_H
