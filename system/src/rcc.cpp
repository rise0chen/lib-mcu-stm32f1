/*************************************************
Copyright (C), 2018-2028, Crise Tech. Co., Ltd.
File name: Rcc.cpp
Author: rise0chen
Version: 1.0
Date: 2018.4.26
Description: 系统时钟 类
Usage:
	rcc.init(9); //HSE晶振9倍频
	rcc.reset(2, APB2_GPIOA); //复位GPIOA
	rcc.cmd(2, APB2_GPIOA, ENABLE);//使能GPIOA时钟
	
History: 
	rise0chen   2018.4.26   编写注释
*************************************************/
#include "Rcc.hpp"

Rcc rcc;
//uint32_t SystemCoreClock;

/*************************************************
Function: Rcc::cmd
Description: 设置外设时钟
Calls: 
Called By: 
Input: 
	bus     外设总线 0:AHB; 1:APB1; 2:APB2
	periph  外设，如APB2_AFIO
	state   使能状态 ENABLE使能 DISABLE失能
Return: void
*************************************************/
void Rcc::cmd(u8 bus, u32 periph, FunctionalState state){
	switch(bus){
		case 0:
			state ? RCC->AHBENR |=periph : RCC->AHBENR &=~periph;
		break;
		case 1:
			state ? RCC->APB1ENR|=periph : RCC->APB1ENR&=~periph;
		break;
		case 2:
			state ? RCC->APB2ENR|=periph : RCC->APB2ENR&=~periph;
		break;
	}
}

/*************************************************
Function: Rcc::reset
Description: 复位外设
Calls: 
Called By: 
Input: 
	bus     外设总线 1:APB1; 2:APB2
	periph  外设，如APB2_AFIO
Return: void
*************************************************/
void Rcc::reset(u8 bus, u32 periph){
	switch(bus){
		case 1:
			RCC->APB1RSTR |= periph;
			RCC->APB1RSTR &= ~periph;
		break;
		case 2:
			RCC->APB2RSTR |= periph;
			RCC->APB2RSTR &= ~periph;
		break;
	}
}

/*************************************************
Function: Rcc::deInit
Description: 把所有时钟寄存器复位
Calls: 
Called By: 
Input: void
Return: void
Other: 不能在这里执行所有外设复位!否则至少引起串口不工作.
*************************************************/
void Rcc::deInit(void){
	RCC->APB1RSTR= 0x00000000;//复位结束
	RCC->APB2RSTR= 0x00000000;

	RCC->AHBENR  = 0x00000014;//睡眠模式闪存和SRAM时钟使能.其他关闭
	RCC->APB2ENR = 0x00000000;//外设时钟关闭
	RCC->APB1ENR = 0x00000000;//外设时钟关闭
	RCC->CR   |= 0x00000001;//使能内部高速时钟HSION
	RCC->CFGR &= 0xF0FF0000;//复位SW[1:0],HPRE[3:0],PPRE1[2:0],PPRE2[2:0],ADCPRE[1:0],MCO[2:0]
	RCC->CR   &= 0xFEF6FFFF;//复位HSEON,CSSON,PLLON
	RCC->CR   &= 0xFFFBFFFF;//复位HSEBYP
	RCC->CFGR &= 0xFF80FFFF;//复位PLLSRC,PLLXTPRE,PLLMUL[3:0]andUSBPRE
	RCC->CIR = 0x00000000;//关闭所有中断
//配置向量表
#ifdef VECT_TAB_RAM
	nvic.setVectorTable(SRAM_BASE,0x0);
#else
	nvic.setVectorTable(FLASH_BASE,0x0);
#endif
}

/*************************************************
Function: Rcc::init
Description: 系统时钟初始化
Calls: 
Called By: 
Input: 
	PLL   倍频数 2~16
Return: void
Other: 不能在这里执行所有外设复位!否则至少引起串口不工作.
*************************************************/
void Rcc::init(u8 PLL){
	deInit();//复位并配置向量表
	RCC->CR   |= (1<<16);//HSE使能
	while(!(RCC->CR & (1<<17)));//等待HSE就绪
	RCC->CFGR=0X00000400;//APB1=DIV2;APB2=DIV1;AHB=DIV1;
	//RCC->CFGR |= (1<<17);//HSE二分频
	PLL -= 2;//抵消2个单位(因为是从2开始的，设置0就是2)
	RCC->CFGR |= (PLL<<18);//设置PLL值2~16
	RCC->CFGR |= (1<<16);//PLL输入始终源 HSE
	FLASH->ACR|= 0x32;//FLASH2个延时周期//0x12
	RCC->CR   |= (1<<24);//PLL使能
	while(!(RCC->CR & (1<<25)));//等待PLL就绪
	RCC->CFGR |= 2;//PLL作为系统时钟
	while(!(RCC->CFGR & (1<<3)));//等待PLL作为系统时钟设置成功
	clkSys  = 72000000;
	clkAhb  = clkSys;
	clkApb1 = clkAhb>>1;
	clkApb2 = clkAhb;
}
