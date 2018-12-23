#ifndef __DMA_H
#define __DMA_H
#include "sys.h"

void dma_reset(DMA_Channel_TypeDef* DMA_Chx);
void dma_configTx(DMA_Channel_TypeDef* DMA_CHx,u32 cpar,u32 cmar,u16 cndtr);//内存发送到外设
void dma_configRx(DMA_Channel_TypeDef* DMA_CHx,u32 cpar,u32 cmar,u16 cndtr);//外设接收到内存
void dma_configADC(u32 cmar, u8 cndtr);//ADC采集


#endif //__DMA_H
