#ifndef __EEPROM_H
#define __EEPROM_H
#include "i2c.hpp"

class Eeprom{
	public:
		u8  pageSize;
		u16 pageMax;
	
		Eeprom(I2c *com, u8 addr, u8 type);
		ErrorStatus  write(u16 addr,void* buf,u16 num);
		ErrorStatus  read(u16 addr,void* buf,u16 num);
	private:
		I2c *com;
		u8  deviceAddr;
		u8  type;
		
		ErrorStatus  writePage(u16 addr, char* pBuf, u8 num);
};

#endif //__EEPROM_H
