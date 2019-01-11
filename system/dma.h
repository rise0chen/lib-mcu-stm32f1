#ifndef __DMA_H
#define __DMA_H
#include "sys.h"

void dma_reset(DMA_Channel_TypeDef* DMA_Chx);
void dma_configTx(DMA_Channel_TypeDef* DMA_CHx,uint32_t cpar,uint32_t cmar,uint16_t cndtr);//内存发送到外设
void dma_configRx(DMA_Channel_TypeDef* DMA_CHx,uint32_t cpar,uint32_t cmar,uint16_t cndtr);//外设接收到内存
void dma_configADC(uint32_t cmar, uint8_t cndtr);//ADC采集


#endif //__DMA_H
