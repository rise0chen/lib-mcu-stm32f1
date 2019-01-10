/*************************************************
Copyright (C), 2018-2028, Crise Tech. Co., Ltd.
File name: Adc.cpp
Author: rise0chen
Version: 1.0
Date: 2018.4.26
Description: Dac 类
Usage:
	adc1.init();//初始化DAC1
History: 
	rise0chen   2018.4.26   编写注释
*************************************************/
#include "dac.h"

DacStruct* dac_new(u8 t){
  DacStruct* self = (DacStruct*)malloc(sizeof(DacStruct));
	switch(t){
		case 1:
			//the=DAC1;
			//RCC_The=APB1_DAC1;
			break;
	}
  return self;
}
void dac_init(DacStruct* self){
	
}
