/*************************************************
Copyright (C), 2018-2028, Crise Tech. Co., Ltd.
File name: transCode.cpp
Author: rise0chen
Version: 1.0
Date: 2018.4.26
Description: 帧传输通信协议
Usage: 
	#include "transCode.hpp"
	utf8ToGbk(in, out);
History: 
	rise0chen   2018.4.26   编写注释
*************************************************/
#include "transCode.hpp"

/*************************************************
Function: Can::utf8ToGbk
Description: UTF8编码转GBK编码(需要用到GT21L16S2Y字库芯片)
Input: 
	bufIn  待转换的UTF8数据
	bufOut 转换出的GBK数据
Return: GBK数据的长度
*************************************************/
unsigned char utf8ToGbk(const char* bufIn, char* bufOut){
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
			*FontRom::CS->O=0;
			FontRom::com->rwByte(0x03);
			FontRom::com->rwByte(gbkaddr>>16);
			FontRom::com->rwByte(gbkaddr>>8);
			FontRom::com->rwByte(gbkaddr);
			bufOut[len_out++] = FontRom::com->rwByte(0x00);
			bufOut[len_out++] = FontRom::com->rwByte(0x00);
			*FontRom::CS->O=1;
		}
	}
	bufOut[len_out] = 0x00;
	return len_out;
}
