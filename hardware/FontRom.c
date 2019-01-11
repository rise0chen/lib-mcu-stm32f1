/*************************************************
Copyright (C), 2018-2028, Crise Tech. Co., Ltd.
File name: FontRom.cpp
Author: rise0chen
Version: 1.0
Date: 2018.4.26
Description: GT21L16S2Y字库IC
Usage: 
	#include "FontRom.hpp"
	FontRom  fontIC(&spi_com, &spi_cs);
History: 
	rise0chen   2018.4.26   编写注释
*************************************************/
#include "FontRom.h"

/*************************************************
Function: fontRom_FontRom
Description: FontRom构造函数
Input: 
	spi_com spi外设
	spi_c是 spi片选引脚
Return: FontRom类
*************************************************/
FontRomStruct* fontRom_FontRom(SpiStruct *spi_com, GpioStruct *spi_cs){
  FontRomStruct* self = (FontRomStruct*)malloc(sizeof(FontRomStruct));
	self->com = spi_com;
	self->CS  = spi_cs;
  return self;
}

/*************************************************
Function: fontRom_init
Description: 字库IC初始化
Input: void
Return: void
*************************************************/
void fontRom_init(FontRomStruct* self){
	gpio_config(self->CS, P_PPO,0,P_2MHz);
}

/*************************************************
Function: fontRom_getWord16x16
Description: 获取GBK编码点阵
Input: 
	gbk   GBK编码
	data  返回的点阵数据
Return: void
*************************************************/
void fontRom_getWord16x16(FontRomStruct* self, uint16_t gbk, uint8_t* data){
	uint32_t  fontaddr=0;
	uint8_t text[2]={(uint8_t)(gbk>>8), (uint8_t)gbk};
	
	if(((text[0]>=0xb0) &&(text[0]<=0xf7))&&(text[1]>=0xa1)){
		fontaddr = (text[0]- 0xb0)*94; 
		fontaddr += (text[1]-0xa1)+846;
		fontaddr = (uint32_t)(fontaddr*32);
	}else if(((text[0]>=0xa1) &&(text[0]<=0xa9))&&(text[1]>=0xa1)){
		fontaddr = (text[0]- 0xa1)*94; 
		fontaddr += (text[1]-0xa1);
		fontaddr = (uint32_t)(fontaddr*32);
	}else if((text[0]>=0xa4) && (text[0]<=0xa8) &&(text[1]>=0xa1)){
		fontaddr = 0;
	}
	
	*self->CS->O=0;
	spi_rwByte(self->com, 0x03);
	spi_rwByte(self->com, (fontaddr&0xff0000)>>16);//地址的高8位,共24位
	spi_rwByte(self->com, (fontaddr&0xff00)>>8);//地址的中8位,共24位
	spi_rwByte(self->com, fontaddr&0xff);//地址的低8位,共24位
	for(int i=0;i<16*16/8;i++){
		data[i] = spi_rwByte(self->com, 0x00);
	}
	*self->CS->O=1;
}

/*************************************************
Function: fontRom_getWord8x16
Description: 获取ASCII编码点阵
Input: 
	ascii ASCII编码
	data  返回的点阵数据
Return: void
*************************************************/
void fontRom_getWord8x16(FontRomStruct* self, uint8_t ascii, uint8_t* data){
	uint32_t  fontaddr=0;
	
	fontaddr = (ascii- 0x20);
	fontaddr = (uint32_t)(fontaddr*16);
	fontaddr = (uint32_t)(fontaddr+0x3b7c0);
	
	*self->CS->O=0;
	spi_rwByte(self->com, 0x03);
	spi_rwByte(self->com, (fontaddr&0xff0000)>>16);//地址的高8位,共24位
	spi_rwByte(self->com, (fontaddr&0xff00)>>8);//地址的中8位,共24位
	spi_rwByte(self->com, fontaddr&0xff);//地址的低8位,共24位
	for(int i=0;i<8*16/8;i++){
		data[i] = spi_rwByte(self->com, 0x00);
	}
	*self->CS->O=1;
}

/*************************************************
Function: fontRom_utf8ToGbk
Description: UTF8编码转GBK编码(需要用到GT21L16S2Y字库芯片)
Input: 
	bufIn  待转换的UTF8数据
	bufOut 转换出的GBK数据
Return: GBK数据的长度
*************************************************/
unsigned char fontRom_utf8ToGbk(FontRomStruct* self, const char* bufIn, char* bufOut){
	unsigned char i_in = 0;
	unsigned char len_out = 0;
	unsigned short unicode;
	unsigned int gbkaddr;
	while(bufIn[i_in] != 0x00){
		unicode = 0;
		if((bufIn[i_in] & 0x80) == 0x00){ //1λ
			bufOut[len_out++] = bufIn[i_in++];
		}
		else if((bufIn[i_in] & 0xE0) == 0xC0){ //2λ
			unicode |= (bufIn[i_in++] & 0x1F) << 6;
			unicode |= (bufIn[i_in++] & 0x3F);
		}
		else if ((bufIn[i_in] & 0xF0) == 0xE0){ //3λ
			unicode |= (bufIn[i_in++] & 0x0F) << 12;
			unicode |= (bufIn[i_in++] & 0x3F) << 6;
			unicode |= (bufIn[i_in++] & 0x3F);
		}
		else{
			return 0;
		}
		
		unsigned char result;
		unsigned int h;
		result=0;
		if(unicode<0xa0) result=1;
		else if(unicode<=0xf7) h=unicode-160;
		else if(unicode<0x2c7) result=1;
		else if(unicode<=0x2c9) h=unicode-160-463;
		else if(unicode<0x2010) result=1;
		else if(unicode<=0x2312) h=unicode-160-463-7494;
		else if(unicode<0x2460) result=1;
		else if(unicode<=0x2642) h=unicode-160-463-7494-333;
		else if(unicode<0x3000) result=1;
		else if(unicode<=0x3017) h=unicode-160-463-7494-333-2493;
		else if(unicode<0x3220) result=1;
		else if(unicode<=0x3229) h=unicode-160-463-7494-333-2493-520;
		else if(unicode<0x4e00) result=1;
		else if(unicode<=0x9b54) h=unicode-160-463-7494-333-2493-520-7126;
		else if(unicode<0x9c7c) result=1;
		else if(unicode<=0x9ce2) h=unicode-160-463-7494-333-2493-520-7126-295;
		else if(unicode<0x9e1f) result=1;
		else if(unicode<=0x9fa0) h=unicode-160-463-7494-333-2493-520-7126-295-316;
		else if(unicode<0xe76c) result=1;
		else if(unicode<=0xe774) h=unicode-160-463-7494-333-2493-520-7126-295-316-18379;
		else if(unicode<0xff00) result=1;
		else if(unicode<=0xff5f) h=unicode-160-463-7494-333-2493-520-7126-295-316-18379-6027;
		else if(unicode<0xffe0) result=1;
		else if(unicode<=0xffe5) h=unicode-160-463-7494-333-2493-520-7126-295-316-18379-6027-128;
		else result=1;
		if(result==0){
			gbkaddr = 0x67d70 + (h<<1);
			*self->CS->O=0;
			spi_rwByte(self->com, 0x03);
			spi_rwByte(self->com, gbkaddr>>16);
			spi_rwByte(self->com, gbkaddr>>8);
			spi_rwByte(self->com, gbkaddr);
			bufOut[len_out++] = spi_rwByte(self->com, 0x00);
			bufOut[len_out++] = spi_rwByte(self->com, 0x00);
			*self->CS->O=1;
		}
	}
	bufOut[len_out] = 0x00;
	return len_out;
}
