#ifndef __USART_H
#define __USART_H
#include "sys.hpp"
#include "Gpio.hpp"
#include "Dma.hpp"
#include "BufRcv.hpp"

class Usart{
	public:
		BufRcv tx;
		BufRcv rx;
		
		Usart(uint8_t t);
		void (*funRx)(char* msg, uint16_t len);
		void init(uint32_t bound,uint8_t s=0x00,uint8_t e=0x0A);
		void printf(char* format, ...);
		void send(char *buf, uint16_t len=0);
		void rcv();
	
	private:
		USART_TypeDef* the;
		uint16_t RCC_GPIO;
		uint32_t RCC_The;
		uint8_t  Px;
		uint8_t  PTX;
		uint8_t  PRX;
		uint8_t  IRQn;
		DMA_Channel_TypeDef* TX_DMA;
		DMA_Channel_TypeDef* RX_DMA;
		uint32_t FLAG_TC;
};
extern Usart usart1;
extern Usart usart2;
extern Usart usart3;

#endif //__USART_H
