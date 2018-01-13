#ifndef __DMA_H
#define __DMA_H
#include "sys.hpp"

namespace dma{
	void DeInit(DMA_Channel_TypeDef* DMA_Chx);

	void TxConfig(DMA_Channel_TypeDef* DMA_CHx,u32 cpar,u32 cmar,u16 cndtr);//内存发送到外设
	void RxConfig(DMA_Channel_TypeDef* DMA_CHx,u32 cpar,u32 cmar,u16 cndtr);//外设接收到内存
	void ADCConfig(u32 cmar, u8 cndtr);//ADC采集
}
#endif //__DMA_H
