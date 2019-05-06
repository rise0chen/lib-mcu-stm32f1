#ifndef __EEPROM_H
#define __EEPROM_H
#include "i2c.hpp"

class Eeprom{
	public:
		uint8_t  pageSize;
		uint16_t pageMax;
	
		Eeprom(I2c *com, uint8_t addr, uint8_t type);
		ErrorStatus  write(uint16_t addr,void* buf,uint16_t num);
		ErrorStatus  read(uint16_t addr,void* buf,uint16_t num);
	private:
		I2c *com;
		uint8_t  deviceAddr;
		uint8_t  type;
		
		ErrorStatus  writePage(uint16_t addr, char* pBuf, uint8_t num);
};

#endif //__EEPROM_H
