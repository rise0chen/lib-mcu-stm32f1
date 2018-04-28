/*************************************************
Copyright (C), 2018-2028, Crise Tech. Co., Ltd.
File name: Flash.cpp
Author: rise0chen
Version: 1.0
Date: 2018.4.26
Description: Flash 类
Usage:
	#include "Flash.hpp"
	flash.read(0xE000ED00, ID, 4); //读CPUID
	flash.read(0X1FFFF7E8, ID, 12);//读设备ID
	flash.write(addrStart, ID, 12);//写入flash
History: 
	rise0chen   2018.4.26   编写注释
*************************************************/
#include "Flash.hpp"

Flash flash;

/*************************************************
Function: Flash::Flash
Description: Flash类的构造函数
Calls: 
Called By: 
Input: 
	s    字符串的起始标志
	e    字符串的终止标志
	fun  字符串接收完成后的执行函数
Return: Flash类
*************************************************/
Flash::Flash(){
	addrStart = 0x0800C000;
	addrEnd   = 0x08010000;
	#if defined (STM32F10X_HD) || defined (STM32F10X_HD_VL) || defined (STM32F10X_CL) || defined (STM32F10X_XL)
		sizePage = 0x800; //2048
	#else
		sizePage = 0x400; //1024
	#endif
}

/*************************************************
Function: Flash::lock
Description: 锁定/解锁flash
Calls: 
Called By: 
Input: en 0解锁 1锁定
Return: void
*************************************************/
void Flash::lock(u8 en){
	if(en){
		FLASH->CR |= 0x00000080;
	}else{
		FLASH->KEYR = 0x45670123;
		FLASH->KEYR = 0xCDEF89AB;
	}
}

/*************************************************
Function: Flash::erasePage
Description: 锁定/解锁flash
Calls: 
Called By: 
Input: en 0解锁 1锁定
Return: void
*************************************************/
void Flash::erasePage(u32 paddr){
	while(FLASH->SR & (1<<0));//忙
	FLASH->CR|=1<<1;//页擦除
	FLASH->AR=paddr;//设置页地址
	FLASH->CR|=1<<6;//开始擦除
	while(FLASH->SR & (1<<0));//忙
	FLASH->CR&=~(1<<1);//清除页擦除标志
}

/*************************************************
Function: Flash::read
Description: 读flash
Calls: 
Called By: 
Input: 
	addr  读取flash的起始地址
	buf   读取flash数据到buf变量中
	num   需要读取的字节数Byte
Return: void
*************************************************/
void Flash::read(u32 addr, void* buf, u16 num){
	u16* p = (u16*)buf;
	
	for(u16 i=0; i<num; i=i+2){
		*(p++) = *(u16*)(addr+i);
	}
}

/*************************************************
Function: Flash::write
Description: 写flash
Calls: 
Called By: 
Input: 
	addr  写入flash的起始地址
	buf   写入buf变量到flash中
	num   需要写入的字节数Byte
Return: void
*************************************************/
void Flash::write(u32 addr, void* buf, u16 num){
	u16* p = (u16*)buf;
	u8 numErase = num/sizePage + 1;
	
	lock(0);
	for(u8 i=0; i<numErase; i++){
		erasePage(addr + (sizePage * i));
	}
	for(u16 i=0; i<num; i=i+2){
		while(FLASH->SR & (1<<0));//忙
		FLASH->CR|=1<<0;//编程使能
		*(u16*)(addr+i) = *(p++);
		while(FLASH->SR & (1<<0));//忙
		FLASH->CR&=~(1<<0);//清除PG位.
	}
	lock(1);
}
