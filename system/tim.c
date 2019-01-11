/*************************************************
Copyright (C), 2018-2028, Crise Tech. Co., Ltd.
File name: Tim.cpp
Author: rise0chen
Version: 1.0
Date: 2018.4.26
Description: Tim 定时器类
Usage:
	#include "Tim.hpp"
	tim1.config(36000, 2000, 1); //2000*36000/72000000=1s
	tim1.clear(); //定时器清零
	tim1.cmd(ENABLE); //定时器使能
History: 
	rise0chen   2018.4.26   编写注释
*************************************************/
#include "tim.h"
/*************************************************
Function: tim_Tim
Description: Tim类的构造函数
Calls: 
Called By: 
Input: t 定时器序号
Return: Tim类
*************************************************/
TimStruct* tim_new(uint8_t t){
  TimStruct* self = (TimStruct*)malloc(sizeof(TimStruct));
	switch(t){
		case 1://high
			self->the=TIM1;
			self->IRQn=TIM1_UP_IRQn;
			break;
		case 2:
			self->the=TIM2;
			self->IRQn=TIM2_IRQn;
			break;
		case 3:
			self->the=TIM3;
			self->IRQn=TIM3_IRQn;
			break;
		case 4:
			self->the=TIM4;
			self->IRQn=TIM4_IRQn;
			break;
		#ifdef STM32F10X_HD
		case 5:
			self->the=TIM5;
			self->IRQn=TIM5_IRQn;
			break;
		case 6://base
			self->the=TIM6;
			self->IRQn=TIM6_IRQn;
			break;
		case 7://base
			self->the=TIM7;
			self->IRQn=TIM7_IRQn;
			break;
		case 8://high
			self->the=TIM8;
			self->IRQn=TIM8_UP_IRQn;
			break;
		#endif //STM32F10X_HD
	}
  return self;
}

/*************************************************
Function: tim_config
Description: 定时器基本设置
Calls: 
Called By: 
Input: 
	div    分频 1~65535
	period 周期 1~65535
	TI     1设置溢出中断
Return: void
*************************************************/
void tim_config(TimStruct* self, uint16_t div, uint16_t period, uint8_t TI){
	self->the->PSC = div -1;//分频
	self->the->ARR = period -1;//周期
	self->the->CR1 = 0x0004;//向上计数等...
//  if ((self->the == TIM1) || (self->the == TIM8)){
//    self->the->RCR = 0;//重复计数
//  }
	if(TI){
		nvic_config(self->IRQn, 2, 0);
		self->the->SR   = 0;//清除中断
		self->the->DIER = 1;//溢出中断
	}
	tim_cmd(self, ENABLE);
}

/*************************************************
Function: tim_config
Description: 定时器引脚输出设置 输出比较模式
Calls: 
Called By: 
Input: 
	channel    引脚输出通道
	dutyFactor PWM占空比
	dTime      死区时间
Return: void
*************************************************/
void tim_configOC(TimStruct* self, uint8_t channel, uint16_t dutyFactor, uint8_t dTime){
	uint16_t tmpccmrx = 0, tmpccer = 0, tmpcr2 = 0;

	self->the->CCER &= (~(uint16_t)(1<<(4*(channel-1))));//禁止引脚输出
	tmpccer = self->the->CCER;
	tmpcr2  = self->the->CR2;
	
	if((self->the == TIM1) || (self->the == TIM8)){ //死区时间
		if(dTime != 0){
			tmpccer &= (uint16_t)(~((uint16_t)(8<<(4*(channel-1)))));
			tmpccer |= (uint16_t)(4<<(4*(channel-1)));//使能互补输出
			tmpcr2  &= (uint16_t)(~((uint16_t)0x0100<<(2*(channel-1))));//死区低电平
			tmpcr2  &= (uint16_t)(~((uint16_t)0x0200<<(2*(channel-1))));//死区低电平
			self->the->BDTR &= ~0xFF;
			self->the->BDTR |= (0x8D00+dTime);//输出使能,死区时间DTime/SYSTEMCLK
		}
	}
	tmpccer &= (~((uint16_t)(1<<(4*(channel-1)))<<1));//高电平有效
	tmpccer |= (1<<(4*(channel-1)));//使能引脚输出
	
	self->the->CCER = tmpccer;
	self->the->CR2  = tmpcr2;
	
	if(channel==1 || channel==2){
		tmpccmrx = self->the->CCMR1;
		tmpccmrx &= (~((uint16_t)0x73<<(8*(channel-1))));
		tmpccmrx |= 0x60<<(8*(channel-1));//PWM1
		self->the->CCMR1 = tmpccmrx;
	}else{
		tmpccmrx = self->the->CCMR2;
		tmpccmrx &= (~((uint16_t)0x73<<(8*(channel-3))));
		tmpccmrx |= 0x60<<(8*(channel-3));//PWM1
		self->the->CCMR2 = tmpccmrx;
	}
	switch(channel){//占空比
		case 1:self->the->CCR1 = (uint16_t)((self->the->ARR + 1)/100*dutyFactor);break;
		case 2:self->the->CCR2 = (uint16_t)((self->the->ARR + 1)/100*dutyFactor);break;
		case 3:self->the->CCR3 = (uint16_t)((self->the->ARR + 1)/100*dutyFactor);break;
		case 4:self->the->CCR4 = (uint16_t)((self->the->ARR + 1)/100*dutyFactor);break;
	}
}

/*************************************************
Function: tim_configIC
Description: 定时器输入比较模式设置
Calls: 
Called By: 
Input: 
	channel 引脚输出通道
	div     预分配
	dTime   死区时间
Return: void
*************************************************/
void tim_configIC(TimStruct* self, uint16_t channel, uint16_t div){

}

/*************************************************
Function: tim_cmd
Description: 定时器开关
Calls: 
Called By: 
Input: state 状态开关 ENABLE开  DISABLE关
Return: void
*************************************************/
void tim_cmd(TimStruct* self, FunctionalState state){
	if(state){
		self->the->EGR |= 1;//计数器归零
		self->the->CNT  = 0;//计数器归零
		self->the->CR1 |= 1;//使能
	}else{
		self->the->CR1 &= ~1;//失能
	}
}

/*************************************************
Function: tim_clear
Description: 定时器清零
Calls: 
Called By: 
Input: void
Return: void
*************************************************/
void tim_clear(TimStruct* self){
	self->the->EGR |= 1;//计数器归零
	self->the->CNT  = 0;//计数器归零
}
