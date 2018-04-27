#ifndef __SPI_H
#define __SPI_H
#include "sys.hpp"
#include "gpio.hpp"

class spi{
	public:
		//bufRcv tx;
		//bufRcv rx;
		
		spi(u8 t);
		void (*funRx)();
		void config();
		void setSpeed(u8 speed);
		u8   rwByte(u8 data);
	
	private:
		SPI_TypeDef* the;
		u16 RCC_GPIO;
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
extern spi spi1;
extern spi spi2;

#endif //__SPI_H
