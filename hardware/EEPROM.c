/*************************************************
Copyright (C), 2018-2028, Crise Tech. Co., Ltd.
File name: Eeprom.cpp
Author: rise0chen
Version: 1.0
Date: 2018.4.26
Description: EEPROM(AT24Cxx系列)
Usage:
	#include "Eeprom.hpp"
	Eeprom at24c02(&i2c2, 0, 128);
	at24c02.write(0x00, data, len);
	at24c02.read(0x00, data, len);
History: 
	rise0chen   2018.4.26   初步完成
*************************************************/
#include "EEPROM.h"
static ErrorStatus  eeprom_writePage(EepromStruct* self, uint16_t addr, char* pBuf, uint8_t num);

/*************************************************
Function: eeprom_Eeprom
Description: Eeprom类的构造函数
Input: 
	self->com  I2C接口
	addr I2C地址
	typ  芯片大小
Return: Eeprom类
*************************************************/
EepromStruct* Eeprom_new(I2cStruct *com, uint8_t addr, uint8_t typ){
  EepromStruct* self = (EepromStruct*)malloc(sizeof(EepromStruct));
  self->com=com;
  self->type=typ;
	self->deviceAddr = 0xA0+addr;
	switch(typ){
		//case  1: self->pageSize=8 ;self->pageMax=16 ;break;
		case  2: self->pageSize=8 ;self->pageMax=16 ;break;
		//case  4: self->pageSize=16;self->pageMax=32 ;break;
		//case  8: self->pageSize=16;self->pageMax=64 ;break;
		//case 16: self->pageSize=16;self->pageMax=128;break;
		//case 32: self->pageSize=32;self->pageMax=128;break;
		case 64: self->pageSize=32;self->pageMax=256;break;
		case 128:self->pageSize=64;self->pageMax=256;break;
		//case 256:self->pageSize=64;self->pageMax=512;break;
	}
  return self;
}

/*************************************************
Function: eeprom_writePage
Description: Eeprom页写入
Input: 
	addr 页首地址
	pBuf 待写入数据
	num  字节长度
Return: 通用错误码
*************************************************/
ErrorStatus eeprom_writePage(EepromStruct* self, uint16_t addr, char* pBuf, uint8_t num){
	i2c_start(self->com);
	if(self->type>16){
		i2c_write(self->com, self->deviceAddr);//发送写命令
		i2c_write(self->com, addr>>8);//发送高地址
	}else{i2c_write(self->com, self->deviceAddr+((addr>>8)<<1));}//发送器件地址0XA0,写数据
	i2c_write(self->com, (uint8_t)addr);//发送低地址
	while(num--){
		if(i2c_write(self->com, *(pBuf++))){
			i2c_stop(self->com);
			return ERROR;
		}
	}
	i2c_stop(self->com);
	delay_ms(10);//EEPROM的写入速度比较慢，加入延迟
	return SUCCESS;
}

/*************************************************
Function: eeprom_write
Description: Eeprom批量写入
Input: 
	addr 首地址
	buf  待写入数据
	num  字节长度
Return: 通用错误码
*************************************************/
ErrorStatus  eeprom_write(EepromStruct* self, uint16_t addr,void* buf,uint16_t num){
	char* pBuf=(char*)buf;
	uint8_t count = 0, NumOfPage = 0, NumOfEnd = 0;

	count = self->pageSize - (addr % self->pageSize);
	if(num<count){
		count=num;
	}else{
		num = num-count;
		NumOfPage = num / self->pageSize;
		NumOfEnd = num % self->pageSize;
	}
	
	if(count != 0){//首页
		reTry=0x10;
		while(eeprom_writePage(self, addr, pBuf, count) & reTry--);
		if(reTry<=0){return OVERTIME;}
		addr += count;
		pBuf += count;
	}
	while(NumOfPage--){//中间页
		reTry=0x10;
		while(eeprom_writePage(self, addr, pBuf, self->pageSize) & reTry--);
		if(reTry<=0){return OVERTIME;}
		addr += self->pageSize;
		pBuf += self->pageSize;
	}
	if(NumOfEnd != 0){//尾页
		reTry=0x10;
		while(eeprom_writePage(self, addr, pBuf, NumOfEnd) & reTry--);
		if(reTry<=0){return OVERTIME;}
	}
	return SUCCESS;
}

/*************************************************
Function: eeprom_read
Description: Eeprom读取数据
Input: 
	addr 首地址
	buf  读取数据保存到buf
	num  字节长度
Return: 通用错误码
*************************************************/
ErrorStatus  eeprom_read(EepromStruct* self, uint16_t addr,void* buf,uint16_t num){
	char* pBuf=(char*)buf;
	i2c_start(self->com);
	if(self->type>16){
		i2c_write(self->com, self->deviceAddr);//发送写命令
		i2c_write(self->com, addr>>8);//发送高地址
	}else{i2c_write(self->com, self->deviceAddr+((addr>>8)<<1));}//发送器件地址0XA0,写数据
	i2c_write(self->com, (uint8_t)addr);//发送低地址
	
	i2c_start(self->com);
	if(i2c_write(self->com, self->deviceAddr+1)){
		i2c_stop(self->com);
		return ERROR;
	}
	while(num--){//Rrad Data
		if(num){
			*(pBuf++) = i2c_read(self->com, 1);
		}else{
			*(pBuf++) = i2c_read(self->com, 0);
		}
	}
	i2c_stop(self->com);
	return SUCCESS;
}
