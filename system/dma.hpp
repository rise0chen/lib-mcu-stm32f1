#ifndef __DMA_H
#define __DMA_H
#include "sys.hpp"

class Dma{
	public:
		void reset(DMA_Channel_TypeDef* DMA_Chx);
		void configTx(DMA_Channel_TypeDef* DMA_CHx,uint32_t cpar,uint32_t cmar,uint16_t cndtr);//内存发送到外设
		void configRx(DMA_Channel_TypeDef* DMA_CHx,uint32_t cpar,uint32_t cmar,uint16_t cndtr);//外设接收到内存
		void configADC(uint32_t cmar, uint8_t cndtr);//ADC采集
};
extern Dma dma;

#endif //__DMA_H
