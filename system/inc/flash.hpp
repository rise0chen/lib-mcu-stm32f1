#ifndef __FLASH_H
#define __FLASH_H
#include "sys.hpp"

#define FLASH_ADDR_START 0x0800C000
#define FLASH_ADDR_END   0x08010000

class Flash{
	public:
		Flash();
		void read(u32 addr, void* buf, u16 num);
		void write(u32 addr, void* buf, u16 num);
	
	private:
		uint16_t sizePage;
	
		void lock(u8 en);
		void erasePage(u32 paddr);
};
extern Flash flash;

#endif //__FLASH_H
