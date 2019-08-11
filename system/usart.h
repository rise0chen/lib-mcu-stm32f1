#ifndef __USART_H
#define __USART_H
#include "dma.h"
#include "gpio.h"
#include "sys.h"

class Usart {
 public:
  uint8_t txBuf[LEN_MID];
  uint8_t rxBuf[LEN_MID];
  uint16_t rxNum;
  void (*funIt)(uint8_t, uint32_t);
  uint8_t IRQn;
  DMA_Channel_TypeDef* RX_DMA;

  Usart(uint8_t t);
  void init(uint32_t bound, uint8_t s = 0x00, uint8_t e = 0x0A);
  void printf(char* format, ...);
  void send(char* buf, uint16_t len = 0);
  void rcv();

 private:
  USART_TypeDef* the;
  uint16_t RCC_GPIO;
  uint32_t RCC_The;
  uint8_t Px;
  uint8_t PTX;
  uint8_t PRX;
  DMA_Channel_TypeDef* TX_DMA;
  uint32_t FLAG_TC;
};
extern Usart usart1;
extern Usart usart2;
extern Usart usart3;

#endif  //__USART_H
