#ifndef __SPI_H
#define __SPI_H
#include "sys.h"
#include "gpio.h"

typedef struct {
  SPI_TypeDef* the;
  uint32_t RCC_The;
  uint8_t  Px;
  uint8_t  PSCK;
  uint8_t  PMISO;
  uint8_t  PMOSI;
  GpioStruct* P_PSCK ;
  GpioStruct* P_PMISO;
  GpioStruct* P_PMOSI;
  uint8_t  IRQn;
  DMA_Channel_TypeDef* TX_DMA;
  DMA_Channel_TypeDef* RX_DMA;
  uint32_t FLAG_TC;
} SpiStruct;  //Spi结构

SpiStruct* spi_new(uint8_t t);
void spi_init(SpiStruct* self);
void spi_setSpeed(SpiStruct* self, uint8_t div);
uint8_t   spi_rwByte(SpiStruct* self, uint8_t data);

#endif //__SPI_H
