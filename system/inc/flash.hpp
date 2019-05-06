#ifndef __FLASH_H
#define __FLASH_H
#include "sys.hpp"

#define FLASH_ADDR_START 0x0800C000
#define FLASH_ADDR_END   0x08010000

class Flash{
	public:
		Flash();
		void read(uint32_t addr, void* buf, uint16_t num);
		void write(uint32_t addr, void* buf, uint16_t num);
	
	private:
		uint16_t sizePage;
	
		void lock(uint8_t en);
		void erasePage(uint32_t paddr);
};
extern Flash flash;

#endif //__FLASH_H
