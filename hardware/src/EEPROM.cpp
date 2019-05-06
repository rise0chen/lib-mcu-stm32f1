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
#include "Eeprom.hpp"

/*************************************************
Function: Eeprom::Eeprom
Description: Eeprom类的构造函数
Input: 
	com  I2C接口
	addr I2C地址
	typ  芯片大小
Return: Eeprom类
*************************************************/
Eeprom::Eeprom(I2c *com, u8 addr, u8 typ):com(com),type(typ){
	deviceAddr = 0xA0+addr;
	switch(typ){
		//case  1: pageSize=8 ;pageMax=16 ;break;
		case  2: pageSize=8 ;pageMax=16 ;break;
		//case  4: pageSize=16;pageMax=32 ;break;
		//case  8: pageSize=16;pageMax=64 ;break;
		//case 16: pageSize=16;pageMax=128;break;
		//case 32: pageSize=32;pageMax=128;break;
		case 64: pageSize=32;pageMax=256;break;
		case 128:pageSize=64;pageMax=256;break;
		//case 256:pageSize=64;pageMax=512;break;
	}
}

/*************************************************
Function: Eeprom::writePage
Description: Eeprom页写入
Input: 
	addr 页首地址
	pBuf 待写入数据
	num  字节长度
Return: 通用错误码
*************************************************/
ErrorStatus Eeprom::writePage(u16 addr, char* pBuf, u8 num){
	com->start();
	if(type>16){
		com->write(deviceAddr);//发送写命令
		com->write(addr>>8);//发送高地址
	}else{com->write(deviceAddr+((addr>>8)<<1));}//发送器件地址0XA0,写数据
	com->write((u8)addr);//发送低地址
	while(num--){
		if(com->write(*(pBuf++))){
			com->stop();
			return error_other;
		}
	}
	com->stop();
	delay_ms(10);//EEPROM的写入速度比较慢，加入延迟
	return error_success;
}

/*************************************************
Function: Eeprom::write
Description: Eeprom批量写入
Input: 
	addr 首地址
	buf  待写入数据
	num  字节长度
Return: 通用错误码
*************************************************/
ErrorStatus  Eeprom::write(u16 addr,void* buf,u16 num){
	char* pBuf=(char*)buf;
	u8 count = 0, NumOfPage = 0, NumOfEnd = 0;

	count = pageSize - (addr % pageSize);
	if(num<count){
		count=num;
	}else{
		num = num-count;
		NumOfPage = num / pageSize;
		NumOfEnd = num % pageSize;
	}
	
	if(count != 0){//首页
		reTry=0x10;
		while(writePage(addr, pBuf, count) & reTry--);
		if(reTry<=0){return error_overtime;}
		addr += count;
		pBuf += count;
	}
	while(NumOfPage--){//中间页
		reTry=0x10;
		while(writePage(addr, pBuf, pageSize) & reTry--);
		if(reTry<=0){return error_overtime;}
		addr += pageSize;
		pBuf += pageSize;
	}
	if(NumOfEnd != 0){//尾页
		reTry=0x10;
		while(writePage(addr, pBuf, NumOfEnd) & reTry--);
		if(reTry<=0){return error_overtime;}
	}
	return error_success;
}

/*************************************************
Function: Eeprom::read
Description: Eeprom读取数据
Input: 
	addr 首地址
	buf  读取数据保存到buf
	num  字节长度
Return: 通用错误码
*************************************************/
ErrorStatus  Eeprom::read(u16 addr,void* buf,u16 num){
	char* pBuf=(char*)buf;
	com->start();
	if(type>16){
		com->write(deviceAddr);//发送写命令
		com->write(addr>>8);//发送高地址
	}else{com->write(deviceAddr+((addr>>8)<<1));}//发送器件地址0XA0,写数据
	com->write((u8)addr);//发送低地址
	
	com->start();
	if(com->write(deviceAddr+1)){
		com->stop();
		return error_other;
	}
	while(num--){//Rrad Data
		if(num){
			*(pBuf++) = com->read(1);
		}else{
			*(pBuf++) = com->read(0);
		}
	}
	com->stop();
	return error_success;
}
