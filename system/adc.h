#ifndef __ADC_H
#define __ADC_H
#include "sys.h"

typedef struct {
		ADC_TypeDef* the;
		uint32_t RCC_The;
} AdcStruct;  //ADC结构

AdcStruct* adc_new(uint8_t t);
void adc_init(AdcStruct* self);
uint16_t  adc_get(AdcStruct* self, uint8_t ch,uint8_t times);
float adc_getTemp(AdcStruct* self, uint8_t times);

#endif //__ADC_H
