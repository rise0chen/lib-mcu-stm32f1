/*************************************************
Copyright (C), 2018-2028, Crise Tech. Co., Ltd.
File name: DS18B20.cpp
Author: rise0chen
Version: 1.0
Date: 2018.4.26
Description: DS18B20温度传感器
Usage: 
	#include "DS18B20.hpp"
	DS18B20 ds18b20(&gpio_default);
	getTemp();
History: 
	rise0chen   2018.4.26   编写注释
*************************************************/
#include "DS18B20.h"
static void ds18b20_init(Ds18b20Struct* self);
static void ds18b20_write(Ds18b20Struct* self, uint8_t dat);
static uint8_t ds18b20_read(Ds18b20Struct* self);

/*************************************************
Function: ds18b20_DS18B20
Description: DS18B20构造函数
Input: P_self->DQ  DS18B20的数据引脚
Return: DS18B20类
*************************************************/
Ds18b20Struct* ds18b20_new(GpioStruct *P_DQ){
  Ds18b20Struct* self = (Ds18b20Struct*)malloc(sizeof(Ds18b20Struct));
	self->DQ = P_DQ;
  return self;
}

/*************************************************
Function: ds18b20_setGPIO
Description: 设置引脚
Input: P_self->DQ  DS18B20的数据引脚
Return: void
*************************************************/
void ds18b20_setGPIO(Ds18b20Struct*self, GpioStruct *P_DQ){
	self->DQ = P_DQ;
}

/*************************************************
Function: ds18b20_init
Description: 初始化引脚
Input: void
Return: void
*************************************************/
void ds18b20_init(Ds18b20Struct*self){
	gpio_config(self->DQ, P_PPO ,0 ,P_2MHz);
	*self->DQ->O=1;
	delay_us(10);
	*self->DQ->O=0;
	delay_us(600);
	*self->DQ->O=1;
	delay_us(50);
	*self->DQ->O=0;
	delay_us(200);
	*self->DQ->O=1;
	delay_us(400);
}

/*************************************************
Function: ds18b20_write
Description: 写单字节数据
Input: data 单字节数据
Return: void
*************************************************/
void ds18b20_write(Ds18b20Struct*self, uint8_t data){
	uint8_t i;
	gpio_config(self->DQ, P_PPO ,0 ,P_2MHz);
	*self->DQ->O=1;
	delay_us(10);
	for(i=0;i<8;i++){
		*self->DQ->O=0;
		if(data&0x01){
			*self->DQ->O=1;
		}else
			*self->DQ->O=0;
		data>>=1;
		delay_us(60);
		*self->DQ->O=1;
		delay_us(10);
	}
	*self->DQ->O=1;
	delay_us(10);
}

/*************************************************
Function: ds18b20_read
Description: 读单字节数据
Input: void
Return: 单字节数据
*************************************************/
uint8_t ds18b20_read(Ds18b20Struct*self){
	uint8_t i,value=0;//一定要给value附初值否则显示会错误
	gpio_config(self->DQ, P_PPO ,0 ,P_2MHz);
	*self->DQ->O=1;
	for(i=0;i<8;i++){
		*self->DQ->O=0;
		value>>=1;
		*self->DQ->O=1;
		gpio_config(self->DQ, P_FIN ,0 ,P_2MHz);
		if(*self->DQ->I==1){
			value|=0x80;
		}
		delay_us(50);
		gpio_config(self->DQ, P_PPO ,0 ,P_2MHz);
		*self->DQ->O=1;
		delay_us(1);
	}
	*self->DQ->O=1;
	delay_us(10);
	return value;
}

/*************************************************
Function: ds18b20_getTemp
Description: 读取温度
Input: P_self->DQ  DS18B20的数据引脚
Return: 摄氏温度(只保留整数部分)
*************************************************/
uint8_t ds18b20_getTemp(Ds18b20Struct*self){
	uint8_t a,b;
	
	ds18b20_init(self);//初始化
	ds18b20_write(self, 0xcc);//发送忽略ROM指令
	ds18b20_write(self, 0x44);//发送温度转换指令
	delay_us(1000);
	ds18b20_init(self);//初始化
	ds18b20_write(self, 0xcc);//发送忽略ROM指令
	ds18b20_write(self, 0xbe);//发读暂存器指令
	a=ds18b20_read(self);//温度的低八位
	b=ds18b20_read(self);//温度的高八位
	b<<=4;//ssss s***；s为标志位s=0表示温度值为正数，s=1温度值为负数
	b+=(a&0xf0)>>4;
	a=a&0x0f;//温度的小数部分
	return b;
}
