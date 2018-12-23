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

Usart usart1(1);
Usart usart2(2);
Usart usart3(3);

/*************************************************
Function: Usart::Usart
Description: Usart类的构造函数
Calls: 
Called By: 
Input: t Usart序号
Return: Usart类
*************************************************/
Usart::Usart(u8 t){
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
void Usart::init(u32 bound){
	u32 clk, temp;
	u16 integer, fraction;
	
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
	nvic_config(IRQn,0,0);
	//DMA设置
	dma_configTx(TX_DMA,(u32)&the->DR,(u32)&txBuf,1);
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
	vsprintf((char*)txBuf, format, ap );
	TX_DMA->CNDTR=std::strlen(txBuf);
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
void Usart::send(uint8_t *buf, u16 len){
	if(len==0){len=std::strlen(buf);}
	while((DMA1->ISR & FLAG_TC)==0);//等待上次结束
	TX_DMA->CCR &= ~1;//关DMA
	std::memcpy(txBuf, buf, len);
	TX_DMA->CNDTR=len;
	DMA1->IFCR  |= FLAG_TC;//清TC中断
	TX_DMA->CCR |= 1;//开DMA
}