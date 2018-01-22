#ifndef __FLASH_H
#define __FLASH_H
#include "sys.hpp"

namespace flash{
	#if defined (STM32F10X_HD) || defined (STM32F10X_HD_VL) || defined (STM32F10X_CL) || defined (STM32F10X_XL)
		#define FLASH_PAGE_SIZE  ((uint16_t)0x800) //2048
	#else
		#define FLASH_PAGE_SIZE  ((uint16_t)0x400) //1024
	#endif
	#define FLASH_START_ADDR   ((uint32_t)0x0800C000)
	#define FLASH_END_ADDR     ((uint32_t)0x08010000)
	
	void Read(u32 addr, void* buf, u16 num);
	void Write(u32 addr, void* buf, u16 num);
}
#endif //__FLASH_H
