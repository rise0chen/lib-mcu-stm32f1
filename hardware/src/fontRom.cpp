#include "fontRom.hpp"

namespace FontRom{
	gpio &CS=gpio_default;
	spi  &com=spi2;
	u32  fontaddr=0;
	
	void getWord16x16(u16 gbk, u8* data){
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
		
		*CS.O=0;
		com.ReadWriteByte(0x03);
		com.ReadWriteByte((fontaddr&0xff0000)>>16);//地址的高8位,共24位
		com.ReadWriteByte((fontaddr&0xff00)>>8);//地址的中8位,共24位
		com.ReadWriteByte(fontaddr&0xff);//地址的低8位,共24位
		for(int i=0;i<16*16/8;i++){
			data[i] = com.ReadWriteByte(0x00);
		}
		*CS.O=1;
	}
	void getWord8x16(u8 ascii, u8* data){
		fontaddr = (ascii- 0x20);
		fontaddr = (u32)(fontaddr*16);
		fontaddr = (u32)(fontaddr+0x3b7c0);
		
		*CS.O=0;
		com.ReadWriteByte(0x03);
		com.ReadWriteByte((fontaddr&0xff0000)>>16);//地址的高8位,共24位
		com.ReadWriteByte((fontaddr&0xff00)>>8);//地址的中8位,共24位
		com.ReadWriteByte(fontaddr&0xff);//地址的低8位,共24位
		for(int i=0;i<8*16/8;i++){
			data[i] = com.ReadWriteByte(0x00);
		}
		*CS.O=1;
	}
}
