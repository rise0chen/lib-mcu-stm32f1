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
#include "spi.h"
/*************************************************
Function: spi_Spi
Description: Spi类的构造函数
Calls: 
Called By: 
Input: t Spi序号
Return: Spi类
*************************************************/
SpiStruct* spi_new(u8 t){
  SpiStruct* self = (SpiStruct*)malloc(sizeof(SpiStruct));
	if(t==1){
		self->the = SPI1;
		self->RCC_The = APB2_SPI1;
		self->Px = PA;
		self->PSCK = 5;
		self->PMISO= 6;
		self->PMOSI= 7;
		self->IRQn=SPI1_IRQn;
		self->TX_DMA = DMA1_Channel4;
		self->RX_DMA = DMA1_Channel5;
		self->FLAG_TC=0x00002000;
	}
	if(t==2){
		self->the = SPI2;
		self->RCC_The = APB1_SPI2;
		self->Px = PB;
		self->PSCK = 13;
		self->PMISO= 14;
		self->PMOSI= 15;
		self->IRQn=SPI2_IRQn;
		self->TX_DMA = DMA1_Channel7;
		self->RX_DMA = DMA1_Channel6;
		self->FLAG_TC=0x02000000;
	}
  return self;
}

/*************************************************
Function: spi_init
Description: SPI初始化
Calls: 
Called By: 
Input: void
Return: void
*************************************************/
void spi_init(SpiStruct* self){
	if(self->the==SPI1){//SPI时钟使能
		rcc_cmd(2, self->RCC_The, ENABLE);
	}else{
		rcc_cmd(1, self->RCC_The, ENABLE);
	}
  self->P_PSCK = gpio_new(self->Px, self->PSCK);
  self->P_PMISO = gpio_new(self->Px, self->PMISO);
  self->P_PMOSI = gpio_new(self->Px, self->PMOSI);
	gpio_config(self->P_PSCK, P_PPAF,0,P_2MHz);
	gpio_config(self->P_PMISO, P_PPAF,0,P_2MHz);
	gpio_config(self->P_PMOSI, P_PPAF,0,P_2MHz);
	//self->the->CR1=0x035F;
	self->the->CR1 |= 0<<10; //全双工模式
	self->the->CR1 |= 1<<9;  //软件nss管理
	self->the->CR1 |= 1<<8;
	self->the->CR1 |= 1<<2;  //SPI主机
	self->the->CR1 |= 0<<11; //8bit数据格式
	self->the->CR1 |= 1<<1;  //空闲模式下SCK为1 CPOL=1
	self->the->CR1 |= 1<<0;  //数据采样从第二个时间边沿开始,CPHA=1
	self->the->CR1 |= 7<<3;  //Fsck=Fpclk1/256
	self->the->CR1 |= 0<<7;  //MSBfirst
	self->the->CR1 |= 1<<6;  //SPI设备使能
	spi_rwByte(self, 0xFF);//启动传输
}

/*************************************************
Function: spi_setSpeed
Description: SPI设置传输速率(SPI速度=fAPB/2^(div+1))
Calls: 
Called By: 
Input: div   0~7越小越快
Return: void
*************************************************/
void spi_setSpeed(SpiStruct* self, u8 div){
	self->the->CR1&=0XFF87;//失能、清速度
	self->the->CR1 |= (div&0X07)<<3;//设置速度
	self->the->CR1 |= 1<<6;//SPI设备使能
}

/*************************************************
Function: spi_rwByte
Description: SPI初始化
Calls: 
Called By: 
Input: data 要写入的字节
Return: 读取到的字节
*************************************************/
u8 spi_rwByte(SpiStruct* self, u8 data){
	reTry = 0XFFFE;
	while((self->the->SR&1<<1)==0){//等待发送区空
		if(reTry-- <= 0)return 0xFF;//超时退出
	}
	self->the->DR = data;//发送一个byte
	
	reTry = 0XFFFE;
	while((self->the->SR&1<<0)==0){//等待接收完一个byte
		if(reTry-- <= 0)return 0xFF;//超时退出
	}
	return self->the->DR;//返回收到的数据
}
