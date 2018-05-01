/*************************************************
Copyright (C), 2018-2028, Crise Tech. Co., Ltd.
File name: BufRcv.cpp
Author: rise0chen
Version: 1.0
Date: 2018.4.26
Description: 用于单字节接收字符串
Usage:
	#include "BufRcv.hpp"
	BufRcv receiver(0x00, 0x0D, fun_rcvfinish);
	char res;
	receiver.rcv(res);
History: 
	rise0chen   2018.4.26   初步完成
*************************************************/
#include "BufRcv.hpp"

/*************************************************
Function: BufRcv::BufRcv
Description: BufRcv类的构造函数
Calls: setFlag, setFunFinish
Called By: usart.cpp
Input: 
	s    字符串的起始标识
	e    字符串的终止标识
	fun  字符串接收完成后的执行函数
Return: BufRcv类
*************************************************/
BufRcv::BufRcv(uint8_t s, uint8_t e, void (*fun)(char* msg, uint16_t len)){
	setFlag(s, e);
	setFunFinish(fun);
}

/*************************************************
Function: BufRcv::setFlag
Description: 设置起始/终止标志
Calls: 
Called By: BufRcv
Input: 
	s    字符串的起始标识
	e    字符串的终止标识
Return: void
*************************************************/
void BufRcv::setFlag(uint8_t s, uint8_t e) {
	flagStart = s;
	flagEnd = e;
	status = 0;
	len = 0;
}

/*************************************************
Function: BufRcv::setFunFinish
Description: 设置接收完成后的执行函数
Calls: 
Called By: BufRcv
Input: 
	fun  字符串接收完成后的执行函数
Return: void
*************************************************/
void BufRcv::setFunFinish(void (*fun)(char* msg, uint16_t len)) {
	funFinish=fun;
}

/*************************************************
Function: BufRcv::rcv
Description: 接收单字节
Calls: 
Called By: usart.cpp
Input: 
	res  需要接收的字节
Return: void
*************************************************/
void BufRcv::rcv(uint8_t res) {
	if((status>>6)!=3){//未开始
		if(flagStart==0x00 && res!=0x00){//无开始标志
			status|=0xC0;
			buf[len++]=res;
		}else if(flagStart!=0x00 && res==flagStart){//开始
			status|=0xC0;
			buf[len++]=res;
		}
	}else if((status>>4)!=0xf){//未结束
		buf[len++]=res;
		if(len >= LEN_MAX-8){
			status=0;
			len=0;//数据过多
		}
		if(res==flagEnd){//结束
			if(flagEnd==0x0D){//0x0D 0x0A 结尾
				status|=0x20;
			}else{
				status|=0x30;
				buf[len]=0x00;
	
				funFinish(buf, len);//执行函数
				
				status=0;
				len=0;
			}
		}else if((status>>4) == 0x0E){
			if(res==0x0A){//0x0D 0x0A 结尾
				status|=0x30;
				buf[len]=0x00;
	
				funFinish(buf, len);//执行函数
				
				status=0;
				len=0;
			}
		}
	}else{
		status=0;
		len=0;
	}
}
