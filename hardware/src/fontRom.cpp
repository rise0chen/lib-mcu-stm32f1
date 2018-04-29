/*************************************************
Copyright (C), 2018-2028, Crise Tech. Co., Ltd.
File name: FontRom.cpp
Author: rise0chen
Version: 1.0
Date: 2018.4.26
Description: GT21L16S2Y字库IC
Usage: 
	#include "FontRom.hpp"
	FontRom  fontIC;
	fontIC.init(spi_com, spi_cs);
History: 
	rise0chen   2018.4.26   编写注释
*************************************************/
#include "FontRom.hpp"

/*************************************************
Function: FontRom::init
Description: 初始化spi接口
Input: 
	spi_com spi外设
	spi_c是 spi片选引脚
Return: void
*************************************************/
void FontRom::init(Spi  *spi_com, Gpio *spi_cs){
	com = spi_com;
	CS  = spi_cs;
}

/*************************************************
Function: FontRom::getWord16x16
Description: 获取GBK编码点阵
Input: 
	gbk   GBK编码
	data  返回的点阵数据
Return: void
*************************************************/
void FontRom::getWord16x16(u16 gbk, u8* data){
	u32  fontaddr=0;
	u8 text[2]={(u8)(gbk>>8), (u8)gbk};
	
	if(((text[0]>=0xb0) &&(text[0]<=0xf7))&&(text[1]>=0xa1)){
		fontaddr = (text[0]- 0xb0)*94; 
		fontaddr += (text[1]-0xa1)+846;
		fontaddr = (u32)(fontaddr*32);
	}else if(((text[0]>=0xa1) &&(text[0]<=0xa9))&&(text[1]>=0xa1)){
		fontaddr = (text[0]- 0xa1)*94; 
		fontaddr += (text[1]-0xa1);
		fontaddr = (u32)(fontaddr*32);
	}else if((text[0]>=0xa4) && (text[0]<=0xa8) &&(text[1]>=0xa1)){
		fontaddr = 0;
	}
	
	*CS->O=0;
	com->rwByte(0x03);
	com->rwByte((fontaddr&0xff0000)>>16);//地址的高8位,共24位
	com->rwByte((fontaddr&0xff00)>>8);//地址的中8位,共24位
	com->rwByte(fontaddr&0xff);//地址的低8位,共24位
	for(int i=0;i<16*16/8;i++){
		data[i] = com->rwByte(0x00);
	}
	*CS->O=1;
}

/*************************************************
Function: FontRom::getWord8x16
Description: 获取ASCII编码点阵
Input: 
	ascii ASCII编码
	data  返回的点阵数据
Return: void
*************************************************/
void FontRom::getWord8x16(u8 ascii, u8* data){
	u32  fontaddr=0;
	
	fontaddr = (ascii- 0x20);
	fontaddr = (u32)(fontaddr*16);
	fontaddr = (u32)(fontaddr+0x3b7c0);
	
	*CS->O=0;
	com->rwByte(0x03);
	com->rwByte((fontaddr&0xff0000)>>16);//地址的高8位,共24位
	com->rwByte((fontaddr&0xff00)>>8);//地址的中8位,共24位
	com->rwByte(fontaddr&0xff);//地址的低8位,共24位
	for(int i=0;i<8*16/8;i++){
		data[i] = com->rwByte(0x00);
	}
	*CS->O=1;
}

/*************************************************
Function: Can::utf8ToGbk
Description: UTF8编码转GBK编码(需要用到GT21L16S2Y字库芯片)
Input: 
	bufIn  待转换的UTF8数据
	bufOut 转换出的GBK数据
Return: GBK数据的长度
*************************************************/
u8 FontRom::utf8ToGbk(const char* bufIn, char* bufOut){
	unsigned char i_in = 0, len_out = 0;
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
			*CS->O=0;
			com->rwByte(0x03);
			com->rwByte(gbkaddr>>16);
			com->rwByte(gbkaddr>>8);
			com->rwByte(gbkaddr);
			bufOut[len_out++] = com->rwByte(0x00);
			bufOut[len_out++] = com->rwByte(0x00);
			*CS->O=1;
		}
	}
	bufOut[len_out] = 0x00;
	return len_out;
}
