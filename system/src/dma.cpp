#include "dma.hpp"
namespace dma{
	void DeInit(DMA_Channel_TypeDef* DMA_Chx){
		//DMA_Chx->CCR  &= (uint16_t)(~DMA_CCR1_EN);
		DMA_Chx->CCR   = 0;
		DMA_Chx->CNDTR = 0;
		DMA_Chx->CPAR  = 0;
		DMA_Chx->CMAR  = 0;
		if(((int)DMA_Chx & 0x0400) == 0 )
			DMA1->IFCR |= (0xf<<(((((int)DMA_Chx|0xff)-8)/20)<<2));
		else
			DMA2->IFCR |= (0xf<<(((((int)DMA_Chx|0xff)-8)/20)<<2));
	}

	void TxConfig(DMA_Channel_TypeDef* DMA_Chx, u32 cpar, u32 cmar, u16 cndtr){
		DMA_Chx->CMAR = cmar;//内存基址
		DMA_Chx->CPAR = cpar;//外设基址
		DMA_Chx->CNDTR= cndtr;//DMA缓存大小
		DMA_Chx->CCR = 0x91;//低优先 数宽8 内增外不 不循环 内到外 无中断
	}
	void RxConfig(DMA_Channel_TypeDef* DMA_Chx, u32 cpar, u32 cmar, u16 cndtr){
		DMA_Chx->CMAR = cmar;//内存基址
		DMA_Chx->CPAR = cpar;//外设基址
		DMA_Chx->CNDTR= cndtr;//DMA缓存大小
		DMA_Chx->CCR = 0x81;//低优先 数宽8 内增外不 不循环 外到内 无中断
	}
	void ADCConfig(u32 cmar, u8 cndtr){
		DMA1_Channel1->CMAR = cmar;//内存基址
		DMA1_Channel1->CPAR = (u32)&ADC1->DR;//外设基址
		DMA1_Channel1->CNDTR= cndtr;//DMA缓存大小
		DMA1_Channel1->CCR = 0x5A1;//低优先 数宽16 内增外不 循环 外到内 无中断
	}
}
