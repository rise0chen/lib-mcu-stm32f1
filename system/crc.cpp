/*************************************************
Copyright (C), 2018-2028, Crise Tech. Co., Ltd.
File name: crc.cpp
Author: rise0chen
Version: 1.0
Date: 2018.4.26
Description: 计算CRC校验值
Usage:
	#include "crc.hpp"
	crc::get16(data, len);
	crc::get32(data, len);
History: 
	rise0chen   2018.4.26   改为namespace格式; 编写注释
*************************************************/
#include "crc.hpp"

uint32_t crc::get32(uint32_t *pBuffer, uint32_t BufferLength){
	uint32_t index = 0;
	CRC->CR = 0x01;//复位CRC->DR
	for(index = 0; index < BufferLength; index++){
		CRC->DR = pBuffer[index];
	}
	return (CRC->DR);
}
