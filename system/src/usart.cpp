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
#include "Usart.hpp"
#include <string.h>
#include <stdarg.h>

Usart usart1(1);
Usart usart2(2);
Usart usart3(3);
extern void USART1_Do(char* msg, uint16_t len);
extern void USART2_Do(char* msg, uint16_t len);
extern void USART3_Do(char* msg, uint16_t len);

/*************************************************
Function: Usart::Usart
Description: Usart类的构造函数
Calls: 
Called By: 
Input: t Usart序号
Return: Usart类
*************************************************/
Usart::Usart(uint8_t t){
	if(t==1){
		the = USART1;
		RCC_GPIO = APB2_GPIOA;
		RCC_The  = APB2_USART1;
		Px  = PA;
		PTX = 9;
		PRX = 10;
		IRQn = USART1_IRQn;
		TX_DMA = DMA1_Channel4;
		RX_DMA = DMA1_Channel5;
		FLAG_TC= 0x00002000;
		funRx  = USART1_Do;
	}
	if(t==2){
		the = USART2;
		RCC_GPIO = APB2_GPIOA;
		RCC_The  = APB1_USART2;
		Px  = PA;
		PTX = 2;
		PRX = 3;
		IRQn = USART2_IRQn;
		TX_DMA = DMA1_Channel7;
		RX_DMA = DMA1_Channel6;
		FLAG_TC= 0x02000000;
		funRx  = USART2_Do;
	}
	if(t==3){
		the = USART3;
		RCC_GPIO = APB2_GPIOB;
		RCC_The  = APB1_USART3;
		Px  = PB;
		PTX = 10;
		PRX = 11;
		IRQn = USART3_IRQn;
		TX_DMA = DMA1_Channel2;
		RX_DMA = DMA1_Channel3;
		FLAG_TC= 0x00000020;
		funRx  = USART3_Do;
	}
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
void Usart::init(uint32_t bound,uint8_t s,uint8_t e){
	uint32_t clk, temp;
	uint16_t integer, fraction;

	rx = BufRcv(s, e, funRx);
	
	RCC->APB2ENR |= RCC_GPIO;//使能PORT口时钟
	//使能串口时钟
	if(the==USART1){
		rcc.reset(2,RCC_The);
		rcc.cmd(2, RCC_The, ENABLE);
		clk = rcc.clkApb2;
	}else{
		rcc.reset(1,RCC_The);
		rcc.cmd(1, RCC_The, ENABLE);
		RCC->APB1ENR |= RCC_The;
		clk = rcc.clkApb1;
	}
	//GPIO端口设置
	Gpio(Px, PTX).config(P_ODAF, 1, P_50MHz);//TX开漏输出
	Gpio(Px, PRX).config(P_UIN);//RX上拉输入
	//波特率设置
	integer=clk/(bound<<4);//得到整数部分
	temp = (clk<<4)/bound;//得到USARTDIV
	fraction=(temp-(integer<<8))>>4;//得到小数部分
	the->BRR=(integer<<4)+fraction;// 波特率设置
	//使能接收中断
	the->CR1 |= 1<<5;//RXNE(1<<6:IDLE)中断使能
	nvic.config(IRQn,0,0);
	//DMA设置
	dma.configTx(TX_DMA,(uint32_t)&the->DR,(uint32_t)&tx.buf,1);
	//dma.configRx(RX_DMA,(uint32_t)&the->DR,(uint32_t)&rx.buf,LEN_MAX);
	the->CR3 |= 0xC0;//DMA使能
	the->CR1 |= 0X200C;//使能,8位数据,无校验位,1位停止,收发
}

/*************************************************
Function: Usart::printf
Description: Usart格式化输出
Calls: 
Called By: 
Input: 与printf相同
Return: void
*************************************************/
void Usart::printf(char *format, ...){
	va_list ap;
	va_start(ap,format);
	while((DMA1->ISR & FLAG_TC)==0);//等待上次结束
	TX_DMA->CCR &= ~1;//关DMA
	vsprintf(tx.buf, format, ap );
	TX_DMA->CNDTR=std::strlen(tx.buf);
	DMA1->IFCR  |= FLAG_TC;//清TC中断
	TX_DMA->CCR |= 1;//开DMA
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
void Usart::send(char *buf, uint16_t len){
	if(len==0){len=std::strlen(buf);}
	while((DMA1->ISR & FLAG_TC)==0);//等待上次结束
	TX_DMA->CCR &= ~1;//关DMA
	std::memcpy(tx.buf, buf, len);
	TX_DMA->CNDTR=len;
	DMA1->IFCR  |= FLAG_TC;//清TC中断
	TX_DMA->CCR |= 1;//开DMA
}

/*************************************************
Function: Usart::rcv
Description: Usart接收单字节
Calls: 
Called By: 
Input: void
Return: void
*************************************************/
void Usart::rcv(){
	uint8_t res;

	if(the->SR&(1<<5)){
		res=the->DR;
		rx.rcv(res);
	}
}

/*************************************************
Function: USART1_IRQHandler
Description: Usart中断函数
Calls: 
Called By: 
Input: void
Return: void
*************************************************/
_C void USART1_IRQHandler(void){
	usart1.rcv();
}
_C void USART2_IRQHandler(void){
	usart2.rcv();
}
_C void USART3_IRQHandler(void){
	usart3.rcv();
}
