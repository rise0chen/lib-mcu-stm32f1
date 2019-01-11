#ifndef __INFRARED_H
#define __INFRARED_H
#include "sys.h"
#include "gpio.h"

typedef struct {
  uint32_t DATA;//定义一个32位数据变量，保存接收码
  uint8_t  IRQ; //定义一个8位数据的变量，用于指示接收标志

  GpioStruct *DQ;
} InfraRedStruct;  //红外线结构

InfraRedStruct* infraRed_new(GpioStruct *P_DQ);
void infraRed_get(InfraRedStruct* self);

#endif //__INFRARED_H
