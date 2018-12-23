#ifndef __FLASH_H
#define __FLASH_H
#include "sys.h"

#if defined (STM32F10X_HD) || defined (STM32F10X_HD_VL) || defined (STM32F10X_CL) || defined (STM32F10X_XL)
	#define PAGE_SIZE (0x800) //2048
#else
	#define PAGE_SIZE (0x400) //1024
#endif

#define FLASH_ADDR_START 0x0800C000
#define FLASH_ADDR_END   0x08010000

void flash_read(u32 addr, void* buf, u16 num);
void flash_write(u32 addr, void* buf, u16 num);
void flash_lock(u8 en);
void flash_erasePage(u32 paddr);

#endif //__FLASH_H
