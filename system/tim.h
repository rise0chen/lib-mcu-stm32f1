#ifndef __TIM_H
#define __TIM_H
#include "sys.h"

typedef struct {
  TIM_TypeDef* the;
  u8 IRQn;
} TimStruct;  //定时器结构

TimStruct* tim_new(u8 t);
void tim_config(TimStruct* self, u16 div, u16 period, u8 TI);
void tim_configOC(TimStruct* self, u8 channel, u16 dutyFactor, u8 dTime);
void tim_configIC(TimStruct* self, u16 channel, u16 div);
void tim_configBD(TimStruct* self, u8 dTime);
void tim_cmd(TimStruct* self, FunctionalState state);
void tim_clear(TimStruct* self);

#endif //__TIM_H
