/*************************************************
Copyright (C), 2018-2028, Crise Tech. Co., Ltd.
File name: Gpio.cpp
Author: rise0chen
Version: 1.0
Date: 2018.4.26
Description: Gpio 类
Usage:
	PA01 = Gpio(PA, 1);//定义引脚
	PA01.config(P_PPO);//设置IO模式
	PA01.output(1); //输出高电平
	PA01.lock(); //锁定,无法改变电平
History: 
	rise0chen   2018.4.26   编写注释
*************************************************/
#include "Gpio.hpp"

Gpio gpio_default(PC, 13);

/*************************************************
Function: Gpio::Gpio
Description: Gpio类的构造函数
Calls: 
Called By: 
Input: 
	x    GPIO组
	n    IO口号
Return: Gpio类
*************************************************/
Gpio::Gpio(u8 x, u8 n){
	Px = x;
	GPIOx = (GPIO_TypeDef*)(GPIOA_BASE+(x<<10));
	Pn = n;
	O  = BITBAND((int)&GPIOx->ODR, n);
	I  = BITBAND((int)&GPIOx->IDR, n);
}

/*************************************************
Function: Gpio::config
Description: 配置IO口模式
Calls: 
Called By: 
Input: 
	mode  输入/输出模式,详见GPIOMode_TypeDef
	data  输出电平高低(仅输出模式有效)
	speed IO口开关速率(仅输出模式有效)
Return: void
*************************************************/
void Gpio::config(GPIOMode_TypeDef mode, s8 data, GPIOSpeed_TypeDef speed){
	uint32_t currentmode = 0x00, pos = 0x00;
	uint32_t tmpreg = 0x00;

	RCC->APB2ENR |= 1<<(Px+2);//时钟
	if((mode & 0x80) == 0){
		currentmode = mode & 0x0F;
		if((mode & 0x10) != 0)
			currentmode |= speed;
		if(Pn<8){
			tmpreg = GPIOx->CRL;
			pos = Pn << 2;
			tmpreg &= ~((0x0F) << pos);//clear
			tmpreg |= (currentmode << pos);//revise
			GPIOx->CRL = tmpreg;//write
		}else{
			tmpreg = GPIOx->CRH;
			pos = (Pn-8) << 2;
			tmpreg &= ~((0x0F) << pos);//clear
			tmpreg |= (currentmode << pos);//revise
			GPIOx->CRH = tmpreg;//write
		}
		if(mode == P_DIN)
			GPIOx->BRR = (1<<Pn);
		if(mode == P_UIN)
			GPIOx->BSRR = (1<<Pn);
		if((mode & 0x10) != 0)
			*this->O = data;
	}else{
		switch((u8)mode){
			case 0x80:
				AFIO->EVCR =(1<<7) | (Px << 0x04) | Pn;
			break;
		}
	}
}

/*************************************************
Function: Gpio::output
Description: IO口输出电平
Calls: 
Called By: 
Input: 
	data  1高电平  0低电平  -1反转
Return: void
*************************************************/
void Gpio::output(s8 data){
	switch(data){
		case 1 :GPIOx->BSRR = (1<<Pn);break;//置1
		case 0 :GPIOx->BRR  = (1<<Pn);break;//置0
		case -1:(GPIOx->ODR & (1<<Pn)) ? (GPIOx->BRR = (1<<Pn)) : (GPIOx->BSRR = (1<<Pn));break;//反转
	}
}

/*************************************************
Function: Gpio::input
Description: 读取IO口电平
Calls: 
Called By: 
Input: void
Return: 1高电平  0低电平
*************************************************/
u8 Gpio::input(void){
	return (GPIOx->IDR & (1<<Pn)) ? 1 : 0;
}

/*************************************************
Function: Gpio::configExti
Description: 配置外部中断
	只针对GPIOA~G;不包括PVD,RTC和USB唤醒这三个
	该函数一次只能配置1个IO口,多个IO口,需多次调用
	该函数会自动开启对应中断,以及屏蔽线
Calls: 
Called By: 
Input: 
	TRIM  触发模式 1下升沿  2上降沿  3任意电平触发
Return: void
*************************************************/
void Gpio::configExti(u8 TRIM){
	u8 EXTADDR, EXTOFFSET, IRQn;

	EXTADDR  = (Pn>>2);//得到中断寄存器组的编号
	EXTOFFSET = (Pn%4)<<2;
	RCC->APB2ENR |= 0x01;//使能io复用时钟
	AFIO->EXTICR[EXTADDR] &= ~(0x000F<<EXTOFFSET);//clear
	AFIO->EXTICR[EXTADDR] |= Px<<EXTOFFSET;//write
	//自动设置
	EXTI->IMR |= (1<<Pn);//开启linePn上的中断
	//EXTI->EMR|=1<<Pn;//不屏蔽linePn上的事件(如果不屏蔽这句,在硬件上是可以的,但是在软件仿真的时候无法进入中断!)
	if(TRIM&0x01) EXTI->FTSR |= 1<<Pn;//linePn上事件下降沿触发
	if(TRIM&0x02) EXTI->RTSR |= 1<<Pn;//linePn上事件上升降沿触发
	if(Pn<5){
		IRQn = 6+Pn;
	}else if(Pn>=5 && Pn<10){
		IRQn = 23;
	}else if(Pn>=10 && Pn<16){
		IRQn = 40;
	}
	nvic.config(IRQn,1,0);
}

/*************************************************
Function: Gpio::lock
Description: 锁定IO口
Calls: 
Called By: 
Input: 
Return: void
*************************************************/
void Gpio::lock(){
	u32 tmp = 0x00010000;
	tmp |= (1<<Pn);
	GPIOx->LCKR = tmp;//1
	GPIOx->LCKR = (1<<Pn);//0
	GPIOx->LCKR = tmp;//1
	tmp = GPIOx->LCKR;
	tmp = GPIOx->LCKR;
}
