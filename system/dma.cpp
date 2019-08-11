/*************************************************
Copyright (C), 2018-2028, Crise Tech. Co., Ltd.
File name: Dma.cpp
Author: rise0chen
Version: 1.0
Date: 2018.4.26
Description: Dma 类
Usage:
        dma.configTx(DMA1_Channel4,(uint32_t)&the->DR,(uint32_t)&tx.buf,1);
        dma.configRx(DMA1_Channel5,(uint32_t)&the->DR,(uint32_t)&rx.buf,LEN_MAX);
        dma.configADC(&data, 1);
History:
        rise0chen   2018.4.26   编写注释
*************************************************/
#include "dma.h"

Dma dma;

/*************************************************
Function: Dma::reset
Description: 复位DMA通道
Calls:
Called By:
Input: ch    DMA通道,如DMA1_Channel5
Return: void
*************************************************/
void Dma::reset(DMA_Channel_TypeDef* ch) {
  // ch->CCR  &= (uint16_t)(~DMA_CCR1_EN);
  ch->CCR = 0;
  ch->CNDTR = 0;
  ch->CPAR = 0;
  ch->CMAR = 0;
  if (((int)ch & 0x0400) == 0)
    DMA1->IFCR |= (0xf << (((((int)ch | 0xff) - 8) / 20) << 2));
  else
    DMA2->IFCR |= (0xf << (((((int)ch | 0xff) - 8) / 20) << 2));
}

/*************************************************
Function: Dma::configTx
Description: 发送模式,从内到外
Calls:
Called By:
Input:
        ch    DMA通道,如DMA1_Channel5
        cpar  内存基址(存储变量)
        cmar  外设基址
        cndtr DMA缓存大小
Return: void
*************************************************/
void Dma::configTx(DMA_Channel_TypeDef* ch,
                   uint32_t cpar,
                   uint32_t cmar,
                   uint16_t cndtr) {
  ch->CMAR = cmar;    //内存基址
  ch->CPAR = cpar;    //外设基址
  ch->CNDTR = cndtr;  // DMA缓存大小
  ch->CCR = 0x91;  //低优先 数宽8 内增外不 不循环 内到外 无中断
}

/*************************************************
Function: Dma::configRx
Description: 接收模式,从外到内
Calls:
Called By:
Input:
        ch    DMA通道,如DMA1_Channel5
        cpar  内存基址(存储变量)
        cmar  外设基址
        cndtr DMA缓存大小
Return: void
*************************************************/
void Dma::configRx(DMA_Channel_TypeDef* ch,
                   uint32_t cpar,
                   uint32_t cmar,
                   uint16_t cndtr) {
  ch->CMAR = cmar;    //内存基址
  ch->CPAR = cpar;    //外设基址
  ch->CNDTR = cndtr;  // DMA缓存大小
  ch->CCR = 0x81;  //低优先 数宽8 内增外不 不循环 外到内 无中断
}

/*************************************************
Function: Dma::configADC
Description: ADC1接收模式,自动保存到cmar
Calls:
Called By:
Input:
        cpar  内存基址(存储变量)
        cndtr DMA缓存大小
Return: void
*************************************************/
void Dma::configADC(uint32_t cmar, uint8_t cndtr) {
  DMA1_Channel1->CMAR = cmar;                 //内存基址
  DMA1_Channel1->CPAR = (uint32_t)&ADC1->DR;  //外设基址
  DMA1_Channel1->CNDTR = cndtr;               // DMA缓存大小
  DMA1_Channel1->CCR = 0x5A1;  //低优先 数宽16 内增外不 循环 外到内 无中断
}
