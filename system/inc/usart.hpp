#ifndef __USART_H
#define __USART_H
#include "sys.hpp"
#include "gpio.hpp"
#include "dma.hpp"
#include "bufRcv.hpp"

class usart{
	
	public:
		bufRcv tx;
		bufRcv rx;
		
		usart(u8 t);
		void (*funRx)(char* msg, u16 len);
		void config(u32 bound,u8 s=0x00,u8 e=0x0A);
		void printf(char* format, ...);
		void send(char *buf, u16 len=0);
		void rcv();
	
	private:
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
extern usart usart1;
extern usart usart2;
extern usart usart3;

#endif //__USART_H
