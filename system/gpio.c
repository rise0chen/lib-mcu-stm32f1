/*************************************************
Copyright (C), 2018-2028, Crise Tech. Co., Ltd.
File name: Gpio.cpp
Author: rise0chen
Version: 1.0
Date: 2018.4.26
Description: GpioStruct 类
Usage:
	PA01 = gpio_new(PA, 1);//定义引脚
	PA01.config(P_PPO);//设置IO模式
	PA01.output(1); //输出高电平
	PA01.lock(); //锁定,无法改变电平
History: 
	rise0chen   2018.4.26   编写注释
*************************************************/
#include "gpio.h"
/*************************************************
Function: gpio_Gpio
Description: Gpio类的构造函数
Calls: 
Called By: 
Input: 
	x    GPIO组
	n    IO口号
Return: Gpio类
*************************************************/
GpioStruct* gpio_new(u8 x, u8 n){
  GpioStruct* self = (GpioStruct*)malloc(sizeof(GpioStruct));
	self->Px = x;
	self->GPIOx = (GPIO_TypeDef*)(GPIOA_BASE+(x<<10));
	self->Pn = n;
	self->O  = BITBAND((int)&self->GPIOx->ODR, n);
	self->I  = BITBAND((int)&self->GPIOx->IDR, n);
  return self;
}

/*************************************************
Function: gpio_config
Description: 配置IO口模式
Calls: 
Called By: 
Input: 
	mode  输入/输出模式,详见GpioModeStruct
	data  输出电平高低(仅输出模式有效)
	speed IO口开关速率(仅输出模式有效)
Return: void
*************************************************/
void gpio_config(GpioStruct* self, GpioModeStruct mode,s8 data,GpioSpeedStruct speed){
	uint32_t currentmode = 0x00, pos = 0x00;
	uint32_t tmpreg = 0x00;

	RCC->APB2ENR |= 1<<(self->Px+2);//时钟
	if((mode & 0x80) == 0){
		currentmode = mode & 0x0F;
		if((mode & 0x10) != 0)
			currentmode |= speed;
		if(self->Pn<8){
			tmpreg = self->GPIOx->CRL;
			pos = self->Pn << 2;
			tmpreg &= ~((0x0F) << pos);//clear
			tmpreg |= (currentmode << pos);//revise
			self->GPIOx->CRL = tmpreg;//write
		}else{
			tmpreg = self->GPIOx->CRH;
			pos = (self->Pn-8) << 2;
			tmpreg &= ~((0x0F) << pos);//clear
			tmpreg |= (currentmode << pos);//revise
			self->GPIOx->CRH = tmpreg;//write
		}
		if(mode == P_DIN)
			self->GPIOx->BRR = (1<<self->Pn);
		if(mode == P_UIN)
			self->GPIOx->BSRR = (1<<self->Pn);
		if((mode & 0x10) != 0)//输出模式
			*self->O = data;//电平
	}else{
		switch((u8)mode){
			case 0x80:
				AFIO->EVCR =(1<<7) | (self->Px << 0x04) | self->Pn;
			break;
		}
	}
}

void gpio_setSpeed(GpioStruct* self, GpioSpeedStruct speed){
  
}

/*************************************************
Function: gpio_output
Description: IO口输出电平
Calls: 
Called By: 
Input: 
	data  1高电平  0低电平  -1反转
Return: void
*************************************************/
void gpio_output(GpioStruct* self, s8 data){
	switch(data){
		case 1 :self->GPIOx->BSRR = (1<<self->Pn);break;//置1
		case 0 :self->GPIOx->BRR  = (1<<self->Pn);break;//置0
		case -1:(self->GPIOx->ODR & (1<<self->Pn)) ? (self->GPIOx->BRR = (1<<self->Pn)) : (self->GPIOx->BSRR = (1<<self->Pn));break;//反转
	}
}

/*************************************************
Function: gpio_input
Description: 读取IO口电平
Calls: 
Called By: 
Input: void
Return: 1高电平  0低电平
*************************************************/
u8 gpio_input(GpioStruct* self){
	return (self->GPIOx->IDR & (1<<self->Pn)) ? 1 : 0;
}

/*************************************************
Function: gpio_configExti
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
void gpio_configExti(GpioStruct* self, u8 TRIM){
	u8 EXTADDR, EXTOFFSET, IRQn;

	EXTADDR  = (self->Pn>>2);//得到中断寄存器组的编号
	EXTOFFSET = (self->Pn%4)<<2;
	RCC->APB2ENR |= 0x01;//使能io复用时钟
	AFIO->EXTICR[EXTADDR] &= ~(0x000F<<EXTOFFSET);//clear
	AFIO->EXTICR[EXTADDR] |= self->Px<<EXTOFFSET;//write
	//自动设置
	EXTI->IMR |= (1<<self->Pn);//开启lineself->Pn上的中断
	//EXTI->EMR|=1<<self->Pn;//不屏蔽lineself->Pn上的事件(如果不屏蔽这句,在硬件上是可以的,但是在软件仿真的时候无法进入中断!)
	if(TRIM&0x01) EXTI->FTSR |= 1<<self->Pn;//lineself->Pn上事件下降沿触发
	if(TRIM&0x02) EXTI->RTSR |= 1<<self->Pn;//lineself->Pn上事件上升降沿触发
	if(self->Pn<5){
		IRQn = 6+self->Pn;
	}else if(self->Pn>=5 && self->Pn<10){
		IRQn = 23;
	}else if(self->Pn>=10 && self->Pn<16){
		IRQn = 40;
	}
	nvic_config(IRQn,3,0);
}

/*************************************************
Function: gpio_lock
Description: 锁定IO口
Calls: 
Called By: 
Input: 
Return: void
*************************************************/
void gpio_lock(GpioStruct* self){
	u32 tmp = 0x00010000;
	tmp |= (1<<self->Pn);
	self->GPIOx->LCKR = tmp;//1
	self->GPIOx->LCKR = (1<<self->Pn);//0
	self->GPIOx->LCKR = tmp;//1
	tmp = self->GPIOx->LCKR;
	tmp = self->GPIOx->LCKR;
}
