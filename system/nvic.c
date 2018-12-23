/*************************************************
Copyright (C), 2018-2028, Crise Tech. Co., Ltd.
File name: Nvic.cpp
Author: rise0chen
Version: 1.0
Date: 2018.4.26
Description: 中断向量 类
Usage:
	nvic_configGroup(2);//2抢占2响应
	nvic_config(IRQn,2,0);//开启外设nvic
	nvic_disable();//关闭所有中断
	nvic_enable(); //开启所有中断
History: 
	rise0chen   2018.4.26   编写注释
*************************************************/
#include "nvic.h"

u8 cfgGroup;

/*************************************************
Function: nvic_setVectorTable
Description: 设置向量表偏移地址
Calls: 
Called By: 
Input: 
	vectTab 基址
	offset  偏移量
Return: void
*************************************************/
void nvic_setVectorTable(u32 vectTab,u32 offset){
	SCB->VTOR = vectTab|offset;//设置NVIC的向量表偏移寄存器
	                           //用于标识向量表是在CODE区还是在RAM区
}

/*************************************************
Function: nvic_configGroup
Description: 设置NVIC分组
Calls: 
Called By: 
Input: group 0~4总共5组
	组划分:
	组0:0位抢占优先级,4位响应优先级
	组1:1位抢占优先级,3位响应优先级
	组2:2位抢占优先级,2位响应优先级
	组3:3位抢占优先级,1位响应优先级
	组4:4位抢占优先级,0位响应优先级
Return: void
*************************************************/
void nvic_configGroup(u8 group){
	cfgGroup=group;
	u32 temp,temp1;
	temp1=(~cfgGroup)&0x07;//取后三位
	temp1<<=8;
	temp=SCB->AIRCR;//读取先前的设置
	temp&=0X0000F8FF;//清空先前分组
	temp|=0X05FA0000;//写入钥匙
	temp|=temp1;
	SCB->AIRCR=temp;//设置分组
}

/*************************************************
Function: nvic_config
Description: 设置NVIC
Calls: 
Called By: 
Input: 
	channel    中断编号
	preemption 抢占优先级,数值越小,越优先
	sub        响应优先级,数值越小,越优先
Return: void
*************************************************/
void nvic_config(u8 channel, u8 preemption, u8 sub){
	u32 temp;
	temp=preemption<<(4-cfgGroup);
	temp|=sub&(0x0f>>cfgGroup);
	temp&=0xf;//取低四位
	NVIC->ISER[channel/32]|=(1<<channel%32);//使能中断位(要清除的话,相反操作就OK)
	NVIC->IP[channel]|=temp<<4;//设置响应优先级和抢断优先级
}

/*************************************************
Function: nvic_disable
Description: 关闭所有中断(但是不包括fault和NMI中断)
Calls: 
Called By: 
Input: void
Return: void
*************************************************/
void nvic_disable(void){
	__ASM volatile("cpsid i");
}

/*************************************************
Function: nvic_enable
Description: 开启所有中断
Calls: 
Called By: 
Input: void
Return: void
*************************************************/
void nvic_enable(void){
	__ASM volatile("cpsie i");
}
