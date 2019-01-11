/*************************************************
Copyright (C), 2018-2028, Crise Tech. Co., Ltd.
File name: ultrasonic.cpp
Author: rise0chen
Version: 1.0
Date: 2018.4.26
Description: HC-SR04超声波测距
Usage:
	#include "Ultrasonic.hpp"
	ultrasonic.init(&self->TRIG, &self->ECHO);
	ultrasonic.ranging(10);
History: 
	rise0chen   2018.4.26   编写注释
*************************************************/
#include "Ultrasonic.h"

/*************************************************
Function: ultrasonic_Ultrasonic
Description: 初始化HC-SR04超声波传感器
Input: 
	self->TRIG  控制端引脚
	self->ECHO  接收端引脚
Return: Ultrasonic类
*************************************************/
UltrasonicStruct* ultrasonic_new(GpioStruct *P_TRIG, GpioStruct *P_ECHO){
  UltrasonicStruct* self = (UltrasonicStruct*)malloc(sizeof(UltrasonicStruct));
	self->TRIG = P_TRIG;
	self->ECHO = P_ECHO;
  return self;
}

/*************************************************
Function: ultrasonic_ranging
Description: HC-SR04超声波测距
Input: num   测num次,取平均值
Return: 距离,毫米数
*************************************************/
uint16_t  ultrasonic_get(UltrasonicStruct* self, uint8_t num){
	uint16_t t=0;
	uint16_t tSum=0;
	
	gpio_config(self->TRIG, P_PPO,0,P_2MHz);
	gpio_config(self->ECHO, P_UIN,0,P_2MHz);
	for(uint8_t i=0;i<num;i++){
		*self->TRIG->O=1;
		delay_ms(10);
		*self->TRIG->O=0;
		
		t=0;
		while((!*self->ECHO->I));
		while(*self->ECHO->I && t<2000){//模块最大可测距3.4m
			delay_us(10);
			t++;
		}
		tSum+=340/2*(10*t);
		delay_ms(60);//防止发射信号对回响信号的影响
	}
	self->distance = tSum/num/1000;
	return self->distance;
}
