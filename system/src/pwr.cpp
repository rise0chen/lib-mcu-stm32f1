/*************************************************
Copyright (C), 2018-2028, Crise Tech. Co., Ltd.
File name: pwr.cpp
Author: rise0chen
Version: 1.0
Date: 2018.4.26
Description: 电源功耗相关
Usage:
	pwr::reset();  //系统软复位
	pwr::sleep(1); //休眠模式
	pwr::stop();   //停止模式
	pwr::standby();//待机模式
History: 
	rise0chen   2018.4.26   编写注释
*************************************************/
#include "pwr.hpp"

static void pwr::WFI_SET(void){
	__WFI();
	__ASM volatile("wfi");
}

/*************************************************
Function: pwr::reset
Description: 系统软复位
Calls: 
Called By: 
Input: void
Return: void
*************************************************/
void pwr::reset(void){
	SCB->AIRCR =0X05FA0000|0x04;
}

/*************************************************
Function: pwr::sleep
Description: 进入睡眠模式(任意中断唤醒)
Calls: 
Called By: 
Input: next  1处理完中断后,继续休眠  0处理完中断后,返回主函数
Return: void
*************************************************/
void pwr::sleep(uint8_t next){
	SCB->SCR &= ~(1<<2);//清除SLEEPDEEP位
	if(next==1){
		SCB->SCR |= 1<<1;//使能SLEEPONEXIT位(中断执行后,继续休眠)
	}else{
		SCB->SCR &= ~(1<<1);//清除SLEEPONEXIT位(中断执行后,退出休眠)
	}
	WFI_SET();//执行WFI指令
}

/*************************************************
Function: pwr::stop
Description: 进入停止模式(外部中断唤醒)
Calls: 
Called By: 
Input: void
Return: void
*************************************************/
void pwr::stop(void){
	SysTick->CTRL = 0;//关闭SysTick
	DEM_CR &= ~(1<<24);//关闭DWT等
	SCB->SCR |= 1<<2;//使能SLEEPDEEP位 (SYS->CTRL)
	PWR->CR &= ~(1<<1);//清除PDDS位
	PWR->CR |= 1<<0;//LPDS置位（唤醒时间长，但低功耗）
	WFI_SET();//执行WFI指令
}

/*************************************************
Function: pwr::standby
Description: 进入待机模式(PA0上升沿唤醒)
Calls: 
Called By: 
Input: void
Return: void
*************************************************/
void pwr::standby(void){
	SCB->SCR |= 1<<2;//使能SLEEPDEEP位 (SYS->CTRL)
	RCC->APB1ENR |= 1<<28;//使能电源时钟
	PWR->CSR |= 1<<8;//WKUP(PA0上升沿唤醒)
	PWR->CR |= 1<<2;//清除Wake-up 标志
	PWR->CR |= 1<<1;//PDDS置位
	WFI_SET();//执行WFI指令,进入待机模式
}
