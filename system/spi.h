#ifndef __SPI_H
#define __SPI_H
#include "sys.h"
#include "gpio.h"

typedef struct {
  SPI_TypeDef* the;
  u32 RCC_The;
  u8  Px;
  u8  PSCK;
  u8  PMISO;
  u8  PMOSI;
  GpioStruct* P_PSCK ;
  GpioStruct* P_PMISO;
  GpioStruct* P_PMOSI;
  u8  IRQn;
  DMA_Channel_TypeDef* TX_DMA;
  DMA_Channel_TypeDef* RX_DMA;
  u32 FLAG_TC;
} SpiStruct;  //Spi结构

SpiStruct* spi_new(u8 t);
void spi_init(SpiStruct* self);
void spi_setSpeed(SpiStruct* self, u8 div);
u8   spi_rwByte(SpiStruct* self, u8 data);

#endif //__SPI_H
