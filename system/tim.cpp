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
#include "Tim.hpp"

Tim tim1(1); //high
Tim tim2(2);
Tim tim3(3);
Tim tim4(4);
#ifdef STM32F10X_HD
Tim tim5(5);
Tim tim6(6); //base
Tim tim7(7); //base
Tim tim8(8); //high
#endif //STM32F10X_HD

/*************************************************
Function: Tim::Tim
Description: Tim类的构造函数
Calls: 
Called By: 
Input: t 定时器序号
Return: Tim类
*************************************************/
Tim::Tim(u8 t){
	switch(t){
		case 1:
			the=TIM1;
			IRQn=TIM1_UP_IRQn;
			break;
		case 2:
			the=TIM2;
			IRQn=TIM2_IRQn;
			break;
		case 3:
			the=TIM3;
			IRQn=TIM3_IRQn;
			break;
		case 4:
			the=TIM4;
			IRQn=TIM4_IRQn;
			break;
		#ifdef STM32F10X_HD
		case 5:
			the=TIM5;
			IRQn=TIM5_IRQn;
			break;
		case 6:
			the=TIM6;
			IRQn=TIM6_IRQn;
			break;
		case 7:
			the=TIM7;
			IRQn=TIM7_IRQn;
			break;
		case 8:
			the=TIM8;
			IRQn=TIM8_UP_IRQn;
			break;
		#endif //STM32F10X_HD
	}
}

/*************************************************
Function: Tim::config
Description: 定时器基本设置
Calls: 
Called By: 
Input: 
	div    分频 1~65535
	period 周期 1~65535
	TI     1设置溢出中断
Return: void
*************************************************/
void Tim::config(u16 div, u16 period, u8 TI){
	the->PSC = div -1;//分频
	the->ARR = period -1;//周期
	the->CR1 = 0x0004;//向上计数等...
//  if ((the == TIM1) || (the == TIM8)){
//    the->RCR = 0;//重复计数
//  }
	if(TI){
		nvic_config(IRQn, 2, 0);
		the->SR   = 0;//清除中断
		the->DIER = 1;//溢出中断
	}
	cmd(ENABLE);
}

/*************************************************
Function: Tim::config
Description: 定时器引脚输出设置 输出比较模式
Calls: 
Called By: 
Input: 
	channel    引脚输出通道
	dutyFactor PWM占空比
	dTime      死区时间
Return: void
*************************************************/
void Tim::configOC(u8 channel, u16 dutyFactor, u8 dTime){
	uint16_t tmpccmrx = 0, tmpccer = 0, tmpcr2 = 0;

	the->CCER &= (~(uint16_t)(1<<(4*(channel-1))));//禁止引脚输出
	tmpccer = the->CCER;
	tmpcr2  = the->CR2;
	
	if((the == TIM1) || (the == TIM8)){ //死区时间
		if(dTime != 0){
			tmpccer &= (uint16_t)(~((uint16_t)(8<<(4*(channel-1)))));
			tmpccer |= (uint16_t)(4<<(4*(channel-1)));//使能互补输出
			tmpcr2  &= (uint16_t)(~((uint16_t)0x0100<<(2*(channel-1))));//死区低电平
			tmpcr2  &= (uint16_t)(~((uint16_t)0x0200<<(2*(channel-1))));//死区低电平
			the->BDTR &= ~0xFF;
			the->BDTR |= (0x8D00+dTime);//输出使能,死区时间DTime/SYSTEMCLK
		}
	}
	tmpccer &= (~((uint16_t)(1<<(4*(channel-1)))<<1));//高电平有效
	tmpccer |= (1<<(4*(channel-1)));//使能引脚输出
	
	the->CCER = tmpccer;
	the->CR2  = tmpcr2;
	
	if(channel==1 || channel==2){
		tmpccmrx = the->CCMR1;
		tmpccmrx &= (~((uint16_t)0x73<<(8*(channel-1))));
		tmpccmrx |= 0x60<<(8*(channel-1));//PWM1
		the->CCMR1 = tmpccmrx;
	}else{
		tmpccmrx = the->CCMR2;
		tmpccmrx &= (~((uint16_t)0x73<<(8*(channel-3))));
		tmpccmrx |= 0x60<<(8*(channel-3));//PWM1
		the->CCMR2 = tmpccmrx;
	}
	switch(channel){//占空比
		case 1:the->CCR1 = (u16)((the->ARR + 1)/100*dutyFactor);break;
		case 2:the->CCR2 = (u16)((the->ARR + 1)/100*dutyFactor);break;
		case 3:the->CCR3 = (u16)((the->ARR + 1)/100*dutyFactor);break;
		case 4:the->CCR4 = (u16)((the->ARR + 1)/100*dutyFactor);break;
	}
}

/*************************************************
Function: Tim::configIC
Description: 定时器输入比较模式设置
Calls: 
Called By: 
Input: 
	channel 引脚输出通道
	div     预分配
	dTime   死区时间
Return: void
*************************************************/
void Tim::configIC(u16 channel, u16 div){

}

/*************************************************
Function: Tim::cmd
Description: 定时器开关
Calls: 
Called By: 
Input: state 状态开关 ENABLE开  DISABLE关
Return: void
*************************************************/
void Tim::cmd(FunctionalState state){
	if(state){
		the->EGR |= 1;//计数器归零
		the->CNT  = 0;//计数器归零
		the->CR1 |= 1;//使能
	}else{
		the->CR1 &= ~1;//失能
	}
}

/*************************************************
Function: Tim::clear
Description: 定时器清零
Calls: 
Called By: 
Input: void
Return: void
*************************************************/
void Tim::clear(){
	the->EGR |= 1;//计数器归零
	the->CNT  = 0;//计数器归零
}
