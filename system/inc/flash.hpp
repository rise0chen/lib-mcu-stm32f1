#ifndef __FLASH_H
#define __FLASH_H
#include "sys.hpp"

class Flash{
	public:
		uint16_t sizePage;
		uint32_t addrStart;
		uint32_t addrEnd;
		
		Flash();
		void read(u32 addr, void* buf, u16 num);
		void write(u32 addr, void* buf, u16 num);
	
	private:
		void lock(u8 en);
		void erasePage(u32 paddr);
};
extern Flash flash;

#endif //__FLASH_H
