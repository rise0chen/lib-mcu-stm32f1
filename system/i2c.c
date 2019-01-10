/*************************************************
Copyright (C), 2018-2028, Crise Tech. Co., Ltd.
File name: I2c.cpp
Author: rise0chen
Version: 1.0
Date: 2018.4.26
Description: I2c 类
Usage:
	#include "I2c.hpp"
	i2c1.init();//初始化
	i2c1.start();//起始信号
	i2c1.write(data);//写数据
	i2c1.waitAck();//等待应答
	data = i2c1.read();//读数据
	i2c1.stop();//停止信号
History: 
	rise0chen   2018.4.26   编写注释
*************************************************/
#include "i2c.h"
/*************************************************
Function: i2c_new
Description: I2c类的构造函数
Calls: 
Called By: 
Input: t I2C序号
Return: I2c类
*************************************************/
I2cStruct* i2c_new(u8 t){
  I2cStruct* self=(I2cStruct*)malloc(sizeof(I2cStruct));
	if(t==1){
		self->the = I2C1;
		self->RCC_The = APB1_I2C1;
		self->SCL = gpio_new(PB, 6);
		self->SDA = gpio_new(PB, 7);
		self->IRQn = I2C1_EV_IRQn;
	}
	if(t==2){
		self->the = I2C2;
		self->RCC_The = APB1_I2C2;
		self->SCL = gpio_new(PB, 10);
		self->SDA = gpio_new(PB, 11);
		self->IRQn = I2C2_EV_IRQn;
	}
  return self;
}

/*************************************************
Function: i2c_setGPIO
Description: 设置I2C引脚
Calls: 
Called By: 
Input: void
Return: void
*************************************************/
void i2c_setGPIO(I2cStruct* self, GpioStruct *P_SCL, GpioStruct *P_SDA){
	self->SCL = P_SCL;
	self->SDA = P_SDA;
}

/*************************************************
Function: i2c_init
Description: I2C初始化
Calls: 
Called By: 
Input: void
Return: void
*************************************************/
void i2c_init(I2cStruct* self){
	#if I2C_SOFE//软件I2C
	gpio_config(self->SCL, P_ODO,0,P_2MHz);
	gpio_config(self->SDA, P_ODO,0,P_2MHz);
	*self->SCL->O=1;
	*self->SDA->O=1;
	#else//硬件I2C外设

	#endif
}

/*************************************************
Function: i2c_start
Description: I2C起始信号
Calls: 
Called By: 
Input: void
Return: void
*************************************************/
void i2c_start(I2cStruct* self){
	#if I2C_SOFE//软件I2C
	gpio_config(self->SDA, P_ODO,0,P_2MHz);
	*self->SCL->O=1; //起始条件：self->SCL线是高电平时，self->SDA 线从高电平向低电平切换
	*self->SDA->O=1;
	delay_us(20); //需要大于4.7us
	*self->SDA->O=0;
	delay_us(20); //需要大于4us
	*self->SCL->O=0;
	delay_us(20);
	#else//硬件I2C外设

	#endif
}

/*************************************************
Function: i2c_stop
Description: I2C停止信号
Calls: 
Called By: 
Input: void
Return: void
*************************************************/
void i2c_stop(I2cStruct* self){
	#if I2C_SOFE//软件I2C
	gpio_config(self->SDA, P_ODO,0,P_2MHz);
	*self->SCL->O=0;
	*self->SDA->O=0;
	delay_us(20);
	*self->SCL->O=1;//停止条件：self->SCL线是高电平时，self->SDA 线由低电平向高电平切换
	delay_us(20);
	*self->SDA->O=1;
	delay_us(20);
	#else//硬件I2C外设

	#endif
}

/*************************************************
Function: i2c_ack
Description: I2C应答信号
Calls: 
Called By: 
Input: en 1应当 0不应答
Return: void
*************************************************/
void i2c_ack(I2cStruct* self, u8 en){
	#if I2C_SOFE//软件I2C
	*self->SCL->O=0;
	gpio_config(self->SDA, P_ODO,0,P_2MHz);
	*self->SDA->O= !en;//self->SDA传输完前8位时，当self->SCL来到第9个高电平时，self->SDA线为低电平，则为有效应答位
	delay_us(20);
	*self->SCL->O=1;
	delay_us(20);
	*self->SCL->O=0;
	delay_us(20);
	#else//硬件I2C外设

	#endif
}

/*************************************************
Function: i2c_waitAck
Description: I2C等待应答信号
Calls: 
Called By: 
Input: void
Return: 通用错误码
*************************************************/
ErrorStatus i2c_waitAck(I2cStruct* self){
	#if I2C_SOFE//软件I2C
	gpio_config(self->SDA, P_UIN,0,P_2MHz);//self->SDA设置为输入
	*self->SDA->O=1;
	delay_us(20);
	*self->SCL->O=1;
	delay_us(20);
	
	reTry = 250;
	while(*self->SDA->I){
		if(reTry-- <= 0){
      i2c_stop(self);
      return OVERTIME;
    }
	}
	*self->SCL->O=0;
	return SUCCESS;
	#else//硬件I2C外设

	#endif
}

/*************************************************
Function: i2c_write
Description: I2C写单字节数据
Calls: 
Called By: 
Input: data 单字节数据
Return: void
*************************************************/
ErrorStatus i2c_write(I2cStruct* self, u8 data){
	u8 i;

	gpio_config(self->SDA, P_ODO,0,P_2MHz);
	*self->SCL->O=0;
	for(i=0;i<8;i++){//I2C总线在self->SCL线为高电平时传输数据，在低电平时更换数据
		if(data&0x80)
			*self->SDA->O=1;
		else
			*self->SDA->O=0;
		data<<=1;
		delay_us(20);
		*self->SCL->O=1;
		delay_us(20);//self->SCL线由高电平变为低电平时需要有0.3us的延迟
		*self->SCL->O=0;
		delay_us(20);//self->SCL线由低电平变为高电平时需要有0.3us的延迟
	}
	return(i2c_waitAck(self));
}

/*************************************************
Function: i2c_read
Description: I2C读单字节数据,并发送应答信号
Calls: 
Called By: 
Input: isAck 0不应答  1应答
Return: 单字节数据
*************************************************/
u8 i2c_read(I2cStruct* self, u8 isAck){
	u8 i,receive = 0;

	gpio_config(self->SDA, P_UIN,0,P_2MHz);
	for(i=0;i<8;i++){
		*self->SCL->O=0;
		delay_us(20);
		*self->SCL->O=1;
		receive<<=1;
		if(*self->SDA->I)
			receive++;
		delay_us(20);
	}
	*self->SCL->O=0;
	delay_us(20);
	i2c_ack(self, isAck);//发送ACK 或 NACK
	return(receive);
}

