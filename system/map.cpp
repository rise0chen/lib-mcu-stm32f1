/*************************************************
Copyright (C), 2018-2028, Crise Tech. Co., Ltd.
File name: Gpio.cpp
Author: rise0chen
Version: 1.0
Date: 2018.4.26
Description: 复用功能重映射
Usage:
	map::JTAG(1);
History: 
	rise0chen   2018.4.26   编写注释
*************************************************/
#include "map.hpp"

/*************************************************
Function: map::JTAG
Description: JTAG模式设置,用于设置JTAG的模式
Calls: 
Called By: 
Input: 
	mode  jtag,swd模式设置  00全使能  01使能SWD  10全关闭
Return: void
*************************************************/
void map::JTAG(uint8_t mode){
	RCC->APB2ENR|= 1;//AF
	AFIO->MAPR &= 0XF8FFFFFF;//清除MAPR的[26:24]
	AFIO->MAPR |= (mode<<25);//设置jtag模式
}
