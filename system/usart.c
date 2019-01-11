/*************************************************
Copyright (C), 2018-2028, Crise Tech. Co., Ltd.
File name: Usart.cpp
Author: rise0chen
Version: 1.0
Date: 2018.4.26
Description: Usart 类
Usage:
	#include "Usart.hpp"
	usart1.config(9600, 0x00, 0x0D);//初始化
	usart1.printf("%x\n", data);
	usart1.send(data);
History: 
	rise0chen   2018.4.26   编写注释
*************************************************/
#include "usart.h"
#include <stdarg.h>

/*************************************************
Function: Usart::Usart
Description: Usart类的构造函数
Calls: 
Called By: 
Input: t Usart序号
Return: Usart类
*************************************************/
UsartStruct* usart_new(uint8_t t){
    UsartStruct* self = (UsartStruct*)malloc(sizeof(UsartStruct));
	if(t==1){
		self->the = USART1;
		self->RCC_GPIO = APB2_GPIOA;
		self->RCC_The  = APB2_USART1;
		self->Px  = PA;
		self->PTX = 9;
		self->PRX = 10;
		self->IRQn = USART1_IRQn;
		self->TX_DMA = DMA1_Channel4;
		self->RX_DMA = DMA1_Channel5;
		self->FLAG_TC= 0x00002000;
	}
	if(t==2){
		self->the = USART2;
		self->RCC_GPIO = APB2_GPIOA;
		self->RCC_The  = APB1_USART2;
		self->Px  = PA;
		self->PTX = 2;
		self->PRX = 3;
		self->IRQn = USART2_IRQn;
		self->TX_DMA = DMA1_Channel7;
		self->RX_DMA = DMA1_Channel6;
		self->FLAG_TC= 0x02000000;
	}
	if(t==3){
		self->the = USART3;
		self->RCC_GPIO = APB2_GPIOB;
		self->RCC_The  = APB1_USART3;
		self->Px  = PB;
		self->PTX = 10;
		self->PRX = 11;
		self->IRQn = USART3_IRQn;
		self->TX_DMA = DMA1_Channel2;
		self->RX_DMA = DMA1_Channel3;
		self->FLAG_TC= 0x00000020;
	}
    return self;
}

/*************************************************
Function: Usart::config
Description: Usart初始化
Calls: 
Called By: 
Input: 
	bound 波特率
	s     起始标识 0x00任意字节
	e     终止标识 0x0D为\r\n  0x0A为\n
Return: void
*************************************************/
void usart_init(UsartStruct* self, uint32_t bound){
	uint32_t clk, temp;
	uint16_t integer, fraction;
	
	RCC->APB2ENR |= self->RCC_GPIO;//使能PORT口时钟
	//使能串口时钟
	if(self->the==USART1){
		rcc_reset(2,self->RCC_The);
		rcc_cmd(2, self->RCC_The, ENABLE);
		clk = rcc_clkApb2;
	}else{
		rcc_reset(1,self->RCC_The);
		rcc_cmd(1, self->RCC_The, ENABLE);
		RCC->APB1ENR |= self->RCC_The;
		clk = rcc_clkApb1;
	}
	//GPIO端口设置
	self->P_TX = gpio_new(self->Px, self->PTX);
  gpio_config(self->P_TX, P_ODAF, 1, P_50MHz);//TX开漏输出
	self->P_RX = gpio_new(self->Px, self->PRX);
  gpio_config(self->P_RX, P_UIN,0,P_2MHz);//RX上拉输入
	//波特率设置
	integer=clk/(bound<<4);//得到整数部分
	temp = (clk<<4)/bound;//得到USARTDIV
	fraction=(temp-(integer<<8))>>4;//得到小数部分
	self->the->BRR=(integer<<4)+fraction;// 波特率设置
	//使能接收中断
	self->the->CR1 |= 1<<5;//RXNE(1<<6:IDLE)中断使能
	nvic_config(self->IRQn,0,0);
	//DMA设置
	dma_configTx(self->TX_DMA,(uint32_t)&self->the->DR,(uint32_t)&self->txBuf,1);
	self->the->CR3 |= 0xC0;//DMA使能
	self->the->CR1 |= 0X200C;//使能,8位数据,无校验位,1位停止,收发
}

/*************************************************
Function: Usart::printf
Description: Usart格式化输出
Calls: 
Called By: 
Input: 与printf相同
Return: void
*************************************************/
void usart_printf(UsartStruct* self, char *format, ...){
	va_list ap;
	va_start(ap,format);
	while((DMA1->ISR & self->FLAG_TC)==0);//等待上次结束
	self->TX_DMA->CCR &= ~1;//关DMA
	vsprintf((char*)self->txBuf, format, ap );
	self->TX_DMA->CNDTR=strlen((char*)self->txBuf);
	DMA1->IFCR  |= self->FLAG_TC;//清TC中断
	self->TX_DMA->CCR |= 1;//开DMA
}

/*************************************************
Function: Usart::send
Description: Usart发送字符串
Calls: 
Called By: 
Input: 
	buf   字符串
	len   要发送的字节长度
Return: void
*************************************************/
void usart_send(UsartStruct* self, uint8_t *buf, uint16_t len){
	if(len==0){len=strlen((char*)buf);}
	while((DMA1->ISR & self->FLAG_TC)==0);//等待上次结束
	self->TX_DMA->CCR &= ~1;//关DMA
	memcpy(self->txBuf, buf, len);
	self->TX_DMA->CNDTR=len;
	DMA1->IFCR  |= self->FLAG_TC;//清TC中断
	self->TX_DMA->CCR |= 1;//开DMA
}
