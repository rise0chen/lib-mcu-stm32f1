#ifndef __ADC_H
#define __ADC_H
#include "sys.h"

typedef struct {
		ADC_TypeDef* the;
		u32 RCC_The;
} AdcStruct;  //ADC结构

AdcStruct* adc_new(u8 t);
void adc_init(AdcStruct* self);
u16  adc_get(AdcStruct* self, u8 ch,u8 times);
float adc_getTemp(AdcStruct* self, u8 times);

#endif //__ADC_H
