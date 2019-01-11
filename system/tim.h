#ifndef __TIM_H
#define __TIM_H
#include "sys.h"

typedef struct {
  TIM_TypeDef* the;
  uint8_t IRQn;
} TimStruct;  //定时器结构

TimStruct* tim_new(uint8_t t);
void tim_config(TimStruct* self, uint16_t div, uint16_t period, uint8_t TI);
void tim_configOC(TimStruct* self, uint8_t channel, uint16_t dutyFactor, uint8_t dTime);
void tim_configIC(TimStruct* self, uint16_t channel, uint16_t div);
void tim_configBD(TimStruct* self, uint8_t dTime);
void tim_cmd(TimStruct* self, FunctionalState state);
void tim_clear(TimStruct* self);

#endif //__TIM_H
