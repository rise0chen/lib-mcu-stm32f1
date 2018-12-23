#ifndef __USART_H
#define __USART_H
#include "sys.h"
#include "Gpio.hpp"
#include "dma.h"

class Usart{
	#define BUF_LEN 256
	public:
		Usart(u8 t);
		void init(u32 bound);
		void printf(char* format, ...);
		void send(uint8_t *buf, u16 len);
	
	private:
		u8 txBuf[BUF_LEN];
		USART_TypeDef* the;
		u16 RCC_GPIO;
		u32 RCC_The;
		u8  Px;
		u8  PTX;
		u8  PRX;
		u8  IRQn;
		DMA_Channel_TypeDef* TX_DMA;
		DMA_Channel_TypeDef* RX_DMA;
		u32 FLAG_TC;
};
extern Usart usart1;
extern Usart usart2;
extern Usart usart3;

#endif //__USART_H
