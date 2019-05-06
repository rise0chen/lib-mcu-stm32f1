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
#include "I2c.hpp"

I2c i2c1(1);
I2c i2c2(2);

/*************************************************
Function: I2c::I2c
Description: I2c类的构造函数
Calls: 
Called By: 
Input: t I2C序号
Return: I2c类
*************************************************/
I2c::I2c(uint8_t t){
	if(t==1){
		the = I2C1;
		RCC_The = APB1_I2C1;
		SCL = new Gpio(PB, 6);
		SDA = new Gpio(PB, 7);
		IRQn = I2C1_EV_IRQn;
	}
	if(t==2){
		the = I2C2;
		RCC_The = APB1_I2C2;
		SCL = new Gpio(PB, 10);
		SDA = new Gpio(PB, 11);
		IRQn = I2C2_EV_IRQn;
	}
}

/*************************************************
Function: I2c::setGPIO
Description: 设置I2C引脚
Calls: 
Called By: 
Input: void
Return: void
*************************************************/
void I2c::setGPIO(Gpio *P_SCL, Gpio *P_SDA){
	SCL = P_SCL;
	SDA = P_SDA;
}

/*************************************************
Function: I2c::init
Description: I2C初始化
Calls: 
Called By: 
Input: void
Return: void
*************************************************/
void I2c::init(void){
	#if I2C_SOFE//软件I2C
	SCL->config(P_ODO);
	SDA->config(P_ODO);
	*SCL->O=1;
	*SDA->O=1;
	#else//硬件I2C外设

	#endif
}

/*************************************************
Function: I2c::start
Description: I2C起始信号
Calls: 
Called By: 
Input: void
Return: void
*************************************************/
void I2c::start(void){
	#if I2C_SOFE//软件I2C
	SDA->config(P_ODO);
	*SCL->O=1; //起始条件：SCL线是高电平时，SDA 线从高电平向低电平切换
	*SDA->O=1;
	delay_us(20); //需要大于4.7us
	*SDA->O=0;
	delay_us(20); //需要大于4us
	*SCL->O=0;
	delay_us(20);
	#else//硬件I2C外设

	#endif
}

/*************************************************
Function: I2c::stop
Description: I2C停止信号
Calls: 
Called By: 
Input: void
Return: void
*************************************************/
void I2c::stop(void){
	#if I2C_SOFE//软件I2C
	SDA->config(P_ODO);
	*SCL->O=0;
	*SDA->O=0;
	delay_us(20);
	*SCL->O=1;//停止条件：SCL线是高电平时，SDA 线由低电平向高电平切换
	delay_us(20);
	*SDA->O=1;
	delay_us(20);
	#else//硬件I2C外设

	#endif
}

/*************************************************
Function: I2c::ack
Description: I2C应答信号
Calls: 
Called By: 
Input: en 1应当 0不应答
Return: void
*************************************************/
void I2c::ack(uint8_t en){
	#if I2C_SOFE//软件I2C
	*SCL->O=0;
	SDA->config(P_ODO);
	*SDA->O= !en;//SDA传输完前8位时，当SCL来到第9个高电平时，SDA线为低电平，则为有效应答位
	delay_us(20);
	*SCL->O=1;
	delay_us(20);
	*SCL->O=0;
	delay_us(20);
	#else//硬件I2C外设

	#endif
}

/*************************************************
Function: I2c::waitAck
Description: I2C等待应答信号
Calls: 
Called By: 
Input: void
Return: 通用错误码
*************************************************/
ErrorStatus I2c::waitAck(void){
	#if I2C_SOFE//软件I2C
	SDA->config(P_UIN);//SDA设置为输入
	*SDA->O=1;
	delay_us(20);
	*SCL->O=1;
	delay_us(20);
	
	reTry = 250;
	while(*SDA->I){
		if(reTry-- <= 0){stop();return error_overtime;}
	}
	*SCL->O=0;
	return error_success;
	#else//硬件I2C外设

	#endif
}

/*************************************************
Function: I2c::write
Description: I2C写单字节数据
Calls: 
Called By: 
Input: data 单字节数据
Return: void
*************************************************/
ErrorStatus I2c::write(uint8_t data){
	uint8_t i;

	SDA->config(P_ODO);
	*SCL->O=0;
	for(i=0;i<8;i++){//I2C总线在SCL线为高电平时传输数据，在低电平时更换数据
		if(data&0x80)
			*SDA->O=1;
		else
			*SDA->O=0;
		data<<=1;
		delay_us(20);
		*SCL->O=1;
		delay_us(20);//SCL线由高电平变为低电平时需要有0.3us的延迟
		*SCL->O=0;
		delay_us(20);//SCL线由低电平变为高电平时需要有0.3us的延迟
	}
	return(waitAck());
}

/*************************************************
Function: I2c::read
Description: I2C读单字节数据,并发送应答信号
Calls: 
Called By: 
Input: isAck 0不应答  1应答
Return: 单字节数据
*************************************************/
uint8_t I2c::read(uint8_t isAck){
	uint8_t i,receive = 0;

	SDA->config(P_UIN);
	for(i=0;i<8;i++){
		*SCL->O=0;
		delay_us(20);
		*SCL->O=1;
		receive<<=1;
		if(*SDA->I)
			receive++;
		delay_us(20);
	}
	*SCL->O=0;
	delay_us(20);
	ack(isAck);//发送ACK 或 NACK
	return(receive);
}

