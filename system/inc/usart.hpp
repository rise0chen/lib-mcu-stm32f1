#ifndef __USART_H
#define __USART_H
#include "sys.hpp"
#include "gpio.hpp"
#include "dma.hpp"

class usart{
	
	public:
		u16  TX_Len,RX_Len;//发送长度、接收长度
		char TX_BUF[LEN_MAX];
		char RX_BUF[LEN_MAX];
		
		usart(u8 t);
		void (*funRx)();
		void Config(u32 bound,u8 s=0x00,u8 e=0x0A);
		void printf(char* format, ...);
		void Send(char *buf, u16 len=0);
		void Receive();
	
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
		u8  start, end;//字符串起止标志
		u8  RX_STA;//b7:开始0;  b6:开始1;  b5:结束0;  b4结束1
};
extern usart usart1;
extern usart usart2;
extern usart usart3;

#endif //__USART_H
