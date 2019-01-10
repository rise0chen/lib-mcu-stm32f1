#ifndef __DAC_H
#define __DAC_H
#include "sys.h"

typedef struct {
		DAC_TypeDef* the;
		u32 RCC_The;
} DacStruct;  //DAC结构

DacStruct* dac_new(u8 t);
void dac_init(DacStruct* self);
#endif //__DAC_H
