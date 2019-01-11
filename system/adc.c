/*************************************************
Copyright (C), 2018-2028, Crise Tech. Co., Ltd.
File name: Adc.cpp
Author: rise0chen
Version: 1.0
Date: 2018.4.26
Description: Adc 类
Usage:
	adc1.init();//初始化ADC1
	adc1.get(0, 10);//采集10次ADC1通道0,并取平均值
History: 
	rise0chen   2018.4.26   编写注释
*************************************************/
#include "adc.h"
/*************************************************
Function: adc_Adc
Description: Adc类的构造函数
Calls: 
Called By: 
Input: t ADC序号
Return: Adc类
*************************************************/
AdcStruct* adc_new(uint8_t t){
  AdcStruct* self = (AdcStruct*)malloc(sizeof(AdcStruct));
	switch(t){
		case 1:
			self->the=ADC1;
			self->RCC_The=APB2_ADC1;
			break;
		case 2:
			self->the=ADC2;
			self->RCC_The=APB2_ADC2;
			break;
		case 3:
			self->the=ADC3;
			self->RCC_The=APB2_ADC3;
			break;
	}
  return self;
}

/*************************************************
Function: adc_init
Description: 初始化ADC
Calls: 
Called By: 
Input: void
Return: void
*************************************************/
void adc_init(AdcStruct* self){
	rcc_cmd(2,self->RCC_The,ENABLE);//ADC时钟使能
	rcc_reset(2,self->RCC_The);//ADC复位
	setMem(&RCC->CFGR, 3<<14, 2<<14);//ADC分频。PCLK2/6=12MHz,ADC最大时钟不能超过14M，否则将导致ADC准确度下降!
	setMem(&self->the->CR1, 0xF<<16, 0<<16);//独立工作模式  
	clearBit(&self->the->CR1,8);//非扫描模式
	clearBit(&self->the->CR2,1);//单次转换模式
	setMem(&self->the->CR2, 7<<17, 7<<17);//软件控制转换
	setBit(&self->the->CR2,20);//使用用外部触发(SWSTART)!!!	必须使用一个事件来触发
	clearBit(&self->the->CR2,11);//右对齐
	setMem(&self->the->SQR1,0XF<<20,0<<20);//1个转换在规则序列中
	self->the->SMPR2=0x3FFFFFFF;//239.5周期,提高采样时间可以提高精确度
	self->the->SMPR1=0x00FFFFFF;
	//setMem(&self->the->SMPR2,7<<(3*1),7<<(3*1));
	setBit(&self->the->CR2,0);//开启AD转换器
	setBit(&self->the->CR2,3);//使能复位校准
	while(self->the->CR2 & (1<<3));//等待校准结束
	setBit(&self->the->CR2,2);//开启AD校准
	while(self->the->CR2&1<<2);//等待校准结束
}

/*************************************************
Function: adc_get
Description: 读取AD转换值
Calls: 
Called By: 
Input: 
	ch    ADC通道(0~17)
	times 采集次数(自动取平均值)
Return: 12bit原始电压值
*************************************************/
uint16_t adc_get(AdcStruct* self, uint8_t ch, uint8_t times){
	uint32_t temp_val=0;
	uint8_t t;
	for(t=0;t<times;t++){
		setMem(&self->the->SQR3, 0x1F<<0, ch<<0);//规则序列1 通道ch
		self->the->CR2|=1<<22;//启动规则转换通道
		while(!(self->the->SR & 1<<1));//等待转换结束
		temp_val+=self->the->DR;
	}
	return(temp_val/times);
}

/*************************************************
Function: adc_getTemp
Description: 读取芯片温度
Calls: 
Called By: 
Input: times 采集次数(自动取平均值)
Return: 摄氏度
*************************************************/
float adc_getTemp(AdcStruct* self, uint8_t times){
	uint16_t value;
	float temperate;
	if(self->the!=ADC1){
    return 0;
  }
	setBit(&self->the->CR2,23);
	value=adc_get(self, 16,times);//通道16采集10次,取平均
	temperate=(float)value*3.3/4096;//电压值
	temperate=(1.43-temperate)/0.0043+25;//转换为温度值
	clearBit(&self->the->CR2,23);
	return temperate;
}
