#ifndef __USART_H
#define __USART_H
#include "sys.h"
#include "gpio.h"
#include "dma.h"

#ifdef __cplusplus
extern "C" {
#endif

#define BUF_LEN 256

typedef struct {
  uint8_t txBuf[BUF_LEN];
  USART_TypeDef* the;
  uint16_t RCC_GPIO;
  uint32_t RCC_The;
  uint8_t  Px;
  uint8_t  PTX;
  uint8_t  PRX;
  GpioStruct* P_RX;
  GpioStruct* P_TX;
  uint8_t  IRQn;
  DMA_Channel_TypeDef* TX_DMA;
  DMA_Channel_TypeDef* RX_DMA;
  uint32_t FLAG_TC;
} UsartStruct;  //串口结构

UsartStruct* usart_new(uint8_t t);
void usart_init(UsartStruct* self, uint32_t bound);
void usart_printf(UsartStruct* self, char* format, ...);
void usart_send(UsartStruct* self, uint8_t *buf, uint16_t len);

#ifdef __cplusplus
}
#endif
#endif //__USART_H
