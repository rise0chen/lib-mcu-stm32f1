#include "EEPROM.hpp"

eeprom::eeprom(i2c &com, u8 typ, u8 addr):com(com),type(typ){
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
ErrorStatus eeprom::WritePage(u16 addr, char* pBuf, u8 num){
	com.Start();
	if(type>16){
		com.Write(deviceAddr);//发送写命令
		com.Write(addr>>8);//发送高地址
	}else{com.Write(deviceAddr+((addr>>8)<<1));}//发送器件地址0XA0,写数据
	com.Write((u8)addr);//发送低地址
	while(num--){
		if(com.Write(*(pBuf++))){
			com.Stop();
			return ERROR;
		}
	}
	com.Stop();
	delay_ms(10);//EEPROM的写入速度比较慢，加入延迟
	return SUCCESS;
}
ErrorStatus  eeprom::Write(u16 addr,void* buf,u16 num){
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
		while(WritePage(addr, pBuf, count) & reTry--);
		if(reTry<=0){return OVERTIME;}
		addr += count;
		pBuf += count;
	}
	while(NumOfPage--){//中间页
		reTry=0x10;
		while(WritePage(addr, pBuf, pageSize) & reTry--);
		if(reTry<=0){return OVERTIME;}
		addr += pageSize;
		pBuf += pageSize;
	}
	if(NumOfEnd != 0){//尾页
		reTry=0x10;
		while(WritePage(addr, pBuf, NumOfEnd) & reTry--);
		if(reTry<=0){return OVERTIME;}
	}
	return SUCCESS;
}
ErrorStatus  eeprom::Read(u16 addr,void* buf,u16 num){
	char* pBuf=(char*)buf;
	com.Start();
	if(type>16){
		com.Write(deviceAddr);//发送写命令
		com.Write(addr>>8);//发送高地址
	}else{com.Write(deviceAddr+((addr>>8)<<1));}//发送器件地址0XA0,写数据
	com.Write((u8)addr);//发送低地址
	
	com.Start();
	if(com.Write(deviceAddr+1)){
		com.Stop();
		return ERROR;
	}
	while(num--){//Rrad Data
		if(num){
			*(pBuf++) = com.Read(1);
		}else{
			*(pBuf++) = com.Read(0);
		}
	}
	com.Stop();
	return SUCCESS;
}
