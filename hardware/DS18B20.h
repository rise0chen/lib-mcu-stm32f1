#ifndef __DS18B20_H
#define __DS18B20_H
#include "sys.h"
#include "gpio.h"

typedef struct {
		GpioStruct *DQ;
} Ds18b20Struct;  //温度传感器结构

Ds18b20Struct* ds18b20_new(GpioStruct *P_DQ);
void ds18b20_setGPIO(Ds18b20Struct* self, GpioStruct *P_DQ);
uint8_t ds18b20_getTemp(Ds18b20Struct* self);
	

#endif //__DS18B20_H
