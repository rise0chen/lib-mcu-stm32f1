/*************************************************
Copyright (C), 2018-2028, Crise Tech. Co., Ltd.
File name: delay.cpp
Author: rise0chen
Version: 1.0
Date: 2018.4.26
Description: 延时函数
Usage:
	#include "delay.hpp"
	delay(10);//粗略延时10ms
	delay_us(10);//精准延时10us
	delay_ms(10);//精准延时10ms
	tic(); //调试计时
	toc(); //结束计时
History: 
	rise0chen   2018.4.26   编写注释
*************************************************/
#include "delay.h"

#define fac_us (72)  //us倍乘数 (rcc_clkSys/1 000 000)
#define fac_ms (fac_us*1000)  //ms倍乘数

/*************************************************
Function: delay
Description: 粗略延时1ms
Input: time 毫秒数
Return: void
*************************************************/
void delay(uint16_t time){
	uint16_t i, j;
	for(i=0; i<time; i++)
		for(j=0; j<9000; j++);//1ms
}

/*************************************************
Function: delay_us
Description: 精确毫秒级延时
Input: nus  毫秒数 0~59652323us
Return: void
*************************************************/
void delay_us(uint32_t nus){
	uint32_t start,end;
	DEM_CR |= (1<<24);
	DWT_CR |= (1<<0);
	start = DWT_CYCCNT;
	end = DWT_CYCCNT + nus*fac_us;
	if(end > start){
		while(DWT_CYCCNT < end);
	}else{
		while(DWT_CYCCNT > end);
		while(DWT_CYCCNT < end);
	}
}


static uint32_t startTime; //tic开始时间
/*************************************************
Function: tic
Description: 开始调试计时
Input: void
Return: void
*************************************************/
void tic(){
	DEM_CR |= (1<<24);
	DWT_CR |= (1<<0);
	startTime = DWT_CYCCNT;
}

/*************************************************
Function: toc
Description: 结束调试计时
Input: way  0时钟数 1微秒数 2毫秒数
Return: void
*************************************************/
uint32_t toc(uint8_t way){
	uint32_t dt;
	dt=DWT_CYCCNT-startTime;
	if(way==1){
		dt=dt/fac_us;
	}else if(way==2){
		dt=dt/fac_ms;
	}
	return(dt);
}
