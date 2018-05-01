/*************************************************
Copyright (C), 2018-2028, Crise Tech. Co., Ltd.
File name: ultrasonic.cpp
Author: rise0chen
Version: 1.0
Date: 2018.4.26
Description: HC-SR04超声波测距
Usage:
	#include "Ultrasonic.hpp"
	ultrasonic.init(&TRIG, &ECHO);
	ultrasonic.ranging(10);
History: 
	rise0chen   2018.4.26   编写注释
*************************************************/
#include "Ultrasonic.hpp"

/*************************************************
Function: Ultrasonic::Ultrasonic
Description: 初始化HC-SR04超声波传感器
Input: 
	TRIG  控制端引脚
	ECHO  接收端引脚
Return: Ultrasonic类
*************************************************/
Ultrasonic::Ultrasonic(Gpio *P_TRIG, Gpio *P_ECHO){
	TRIG = P_TRIG;
	ECHO = P_ECHO;
}

/*************************************************
Function: Ultrasonic::ranging
Description: HC-SR04超声波测距
Input: num   测num次,取平均值
Return: 距离,毫米数
*************************************************/
u32  Ultrasonic::ranging(u8 num){
	u8 i;
	u16 j=0;
	u32 Ultr_Temp=0;
	
	TRIG->config(P_PPO);
	ECHO->config(P_UIN);
	for(i=0;i<num;i++){
		*TRIG->O=1;
		delay_ms(10);
		*TRIG->O=0;
		while((!*ECHO->I));
		while(*ECHO->I){
			delay_us(10);
			j++;
		}
		Ultr_Temp+=340/2*j*10;//模块最大可测距3m 
		j=0;
		delay_ms(60);//防止发射信号对回响信号的影响
	}
	distance = Ultr_Temp/num/1000;
	return distance;
}
