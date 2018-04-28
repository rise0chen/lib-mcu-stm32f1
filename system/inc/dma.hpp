#ifndef __DMA_H
#define __DMA_H
#include "sys.hpp"

class Dma{
	public:
		void reset(DMA_Channel_TypeDef* DMA_Chx);
		void configTx(DMA_Channel_TypeDef* DMA_CHx,u32 cpar,u32 cmar,u16 cndtr);//内存发送到外设
		void configRx(DMA_Channel_TypeDef* DMA_CHx,u32 cpar,u32 cmar,u16 cndtr);//外设接收到内存
		void configADC(u32 cmar, u8 cndtr);//ADC采集
};
extern Dma dma;

#endif //__DMA_H
