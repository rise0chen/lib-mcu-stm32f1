/*************************************************
Copyright (C), 2018-2028, Crise Tech. Co., Ltd.
File name: DS18B20.cpp
Author: rise0chen
Version: 1.0
Date: 2018.4.26
Description: DS18B20温度传感器
Usage: 
	#include "DS18B20.hpp"
	getTemp(&DQ);
History: 
	rise0chen   2018.4.26   编写注释
*************************************************/
#include "DS18B20.hpp"

DS18B20 ds18b20;

/*************************************************
Function: DS18B20::init
Description: 初始化引脚
Input: P_DQ  DS18B20的数据引脚
Return: void
*************************************************/
void DS18B20::init(Gpio *P_DQ){
	DQ = P_DQ;
	DQ->config(P_PPO);
	*DQ->O=1;
	delay_us(10);
	*DQ->O=0;
	delay_us(600);
	*DQ->O=1;
	delay_us(50);
	*DQ->O=0;
	delay_us(200);
	*DQ->O=1;
	delay_us(400);
}

/*************************************************
Function: DS18B20::write
Description: 写单字节数据
Input: data 单字节数据
Return: void
*************************************************/
void DS18B20::write(u8 data){
	u8 i;
	DQ->config(P_PPO);
	*DQ->O=1;
	delay_us(10);
	for(i=0;i<8;i++){
		*DQ->O=0;
		if(data&0x01){
			*DQ->O=1;
		}else
			*DQ->O=0;
		data>>=1;
		delay_us(60);
		*DQ->O=1;
		delay_us(10);
	}
	*DQ->O=1;
	delay_us(10);
}

/*************************************************
Function: DS18B20::read
Description: 读单字节数据
Input: void
Return: 单字节数据
*************************************************/
u8 DS18B20::read(void){
	u8 i,value=0;//一定要给value附初值否则显示会错误
	DQ->config(P_PPO);
	*DQ->O=1;
	for(i=0;i<8;i++){
		*DQ->O=0;
		value>>=1;
		*DQ->O=1;
		DQ->config(P_FIN);
		if(*DQ->I==1){
			value|=0x80;
		}
		delay_us(50);
		DQ->config(P_PPO);
		*DQ->O=1;
		delay_us(1);
	}
	*DQ->O=1;
	delay_us(10);
	return value;
}

/*************************************************
Function: DS18B20::getTemp
Description: 读取温度
Input: P_DQ  DS18B20的数据引脚
Return: 摄氏温度(只保留整数部分)
*************************************************/
u8 DS18B20::getTemp(Gpio *P_DQ){
	u8 a,b;
	
	init(P_DQ);//初始化
	write(0xcc);//发送忽略ROM指令
	write(0x44);//发送温度转换指令
	delay_us(1000);
	init(P_DQ);//初始化
	write(0xcc);//发送忽略ROM指令
	write(0xbe);//发读暂存器指令
	a=read();//温度的低八位
	b=read();//温度的高八位
	b<<=4;//ssss s***；s为标志位s=0表示温度值为正数，s=1温度值为负数
	b+=(a&0xf0)>>4;
	a=a&0x0f;//温度的小数部分
	return b;
}
