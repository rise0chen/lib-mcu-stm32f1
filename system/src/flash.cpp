#include "flash.hpp"

namespace flash{
	void Lock(u8 en){
		if(en){
			FLASH->CR |= 0x00000080;
		}else{
			FLASH->KEYR = 0x45670123;
			FLASH->KEYR = 0xCDEF89AB;
		}
	}
	void ErasePage(u32 paddr){
		while(FLASH->SR & (1<<0));//忙
		FLASH->CR|=1<<1;//页擦除
		FLASH->AR=paddr;//设置页地址
		FLASH->CR|=1<<6;//开始擦除
		while(FLASH->SR & (1<<0));//忙
		FLASH->CR&=~(1<<1);//清除页擦除标志
	}
	
	//flash::Read(0X1FFFF7E8, ID, 12);//读设备ID
	void Read(u32 addr, void* buf, u16 num){
		u16* p = (u16*)buf;
		
		for(u16 i=0; i<num; i=i+2){
			*(p++) = *(u16*)(addr+i);
		}
	}
	void Write(u32 addr, void* buf, u16 num){
		u16* p = (u16*)buf;
		u8 numErase = num/FLASH_PAGE_SIZE + 1;
		
		Lock(0);
		for(u8 i=0; i<numErase; i++){
			ErasePage(addr + (FLASH_PAGE_SIZE * i));
		}
		for(u16 i=0; i<num; i=i+2){
			while(FLASH->SR & (1<<0));//忙
			FLASH->CR|=1<<0;//编程使能
			*(u16*)(addr+i) = *(p++);
			while(FLASH->SR & (1<<0));//忙
			FLASH->CR&=~(1<<0);//清除PG位.
		}
		Lock(1);
	}
}
