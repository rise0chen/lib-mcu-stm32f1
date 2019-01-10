#ifndef __USART_H
#define __USART_H
#include "sys.h"
#include "gpio.h"
#include "dma.h"

#define BUF_LEN 256

typedef struct {
  u8 txBuf[BUF_LEN];
  USART_TypeDef* the;
  u16 RCC_GPIO;
  u32 RCC_The;
  u8  Px;
  u8  PTX;
  u8  PRX;
  GpioStruct* P_RX;
  GpioStruct* P_TX;
  u8  IRQn;
  DMA_Channel_TypeDef* TX_DMA;
  DMA_Channel_TypeDef* RX_DMA;
  u32 FLAG_TC;
} UsartStruct;  //串口结构

UsartStruct* usart_new(u8 t);
void usart_init(UsartStruct* self, u32 bound);
void usart_printf(UsartStruct* self, char* format, ...);
void usart_send(UsartStruct* self, uint8_t *buf, u16 len);

#endif //__USART_H
