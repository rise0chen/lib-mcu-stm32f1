#ifndef __TIM_H
#define __TIM_H
#include "sys.h"

class Tim {
 public:
  Tim(uint8_t t);
  void config(uint16_t div, uint16_t period, uint8_t TI = 0);
  void configOC(uint8_t channel, uint16_t dutyFactor, uint8_t dTime = 0);
  void configIC(uint16_t channel, uint16_t div);
  void configBD(uint8_t dTime);
  void cmd(FunctionalState state);
  void clear();

 private:
  TIM_TypeDef* the;
  uint8_t IRQn;
};
extern Tim tim1;
extern Tim tim2;
extern Tim tim3;
extern Tim tim4;
#ifdef STM32F10X_HD
extern Tim tim5;
extern Tim tim6;
extern Tim tim7;
extern Tim tim8;
#endif  // STM32F10X_HD
#endif  //__TIM_H
