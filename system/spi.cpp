/*************************************************
Copyright (C), 2018-2028, Crise Tech. Co., Ltd.
File name: Spi.cpp
Author: rise0chen
Version: 1.0
Date: 2018.4.26
Description: Spi 类
Usage:
	#include "Spi.hpp"
	spi1.init();//初始化
	spi1.rwByte();//读写数据
History: 
	rise0chen   2018.4.26   编写注释
*************************************************/
#include "Spi.hpp"

Spi spi1(1);
Spi spi2(2);

/*************************************************
Function: Spi::Spi
Description: Spi类的构造函数
Calls: 
Called By: 
Input: t Spi序号
Return: Spi类
*************************************************/
Spi::Spi(u8 t){
	if(t==1){
		the = SPI1;
		RCC_The = APB2_SPI1;
		Px = PA;
		PSCK = 5;
		PMISO= 6;
		PMOSI= 7;
		IRQn=SPI1_IRQn;
		TX_DMA = DMA1_Channel4;
		RX_DMA = DMA1_Channel5;
		FLAG_TC=0x00002000;
	}
	if(t==2){
		the = SPI2;
		RCC_The = APB1_SPI2;
		Px = PB;
		PSCK = 13;
		PMISO= 14;
		PMOSI= 15;
		IRQn=SPI2_IRQn;
		TX_DMA = DMA1_Channel7;
		RX_DMA = DMA1_Channel6;
		FLAG_TC=0x02000000;
	}
}

/*************************************************
Function: Spi::init
Description: SPI初始化
Calls: 
Called By: 
Input: void
Return: void
*************************************************/
void Spi::init(){
	if(the==SPI1){//SPI时钟使能
		rcc.cmd(2, RCC_The, ENABLE);
	}else{
		rcc.cmd(1, RCC_The, ENABLE);
	}
	Gpio(Px, PSCK).config(P_PPAF);
	Gpio(Px, PMISO).config(P_PPAF);
	Gpio(Px, PMOSI).config(P_PPAF);
	//the->CR1=0x035F;
	the->CR1 |= 0<<10; //全双工模式
	the->CR1 |= 1<<9;  //软件nss管理
	the->CR1 |= 1<<8;
	the->CR1 |= 1<<2;  //SPI主机
	the->CR1 |= 0<<11; //8bit数据格式
	the->CR1 |= 1<<1;  //空闲模式下SCK为1 CPOL=1
	the->CR1 |= 1<<0;  //数据采样从第二个时间边沿开始,CPHA=1
	the->CR1 |= 7<<3;  //Fsck=Fpclk1/256
	the->CR1 |= 0<<7;  //MSBfirst
	the->CR1 |= 1<<6;  //SPI设备使能
	rwByte(0xFF);//启动传输
}

/*************************************************
Function: Spi::setSpeed
Description: SPI设置传输速率(SPI速度=fAPB/2^(div+1))
Calls: 
Called By: 
Input: div   0~7越小越快
Return: void
*************************************************/
void Spi::setSpeed(u8 div){
	the->CR1&=0XFF87;//失能、清速度
	the->CR1 |= (div&0X07)<<3;//设置速度
	the->CR1 |= 1<<6;//SPI设备使能
}

/*************************************************
Function: Spi::rwByte
Description: SPI初始化
Calls: 
Called By: 
Input: data 要写入的字节
Return: 读取到的字节
*************************************************/
u8 Spi::rwByte(u8 data){
	reTry = 0XFFFE;
	while((the->SR&1<<1)==0){//等待发送区空
		if(reTry-- <= 0)return 0xFF;//超时退出
	}
	the->DR = data;//发送一个byte
	
	reTry = 0XFFFE;
	while((the->SR&1<<0)==0){//等待接收完一个byte
		if(reTry-- <= 0)return 0xFF;//超时退出
	}
	return the->DR;//返回收到的数据
}
