#ifndef __EEPROM_H
#define __EEPROM_H
#include "i2c.hpp"

class eeprom{
	public:
		i2c &com;
		u8  pageSize;
		
		eeprom(i2c &com, u8 type, u8 deviceAddr);
		ErrorStatus  Write(u16 addr,void* buf,u16 num);
		ErrorStatus  Read(u16 addr,void* buf,u16 num);
	private:
		u8  deviceAddr;
		u8  type;
		u16 pageMax;
		
		ErrorStatus  WritePage(u16 addr, char* pBuf, u8 num);
};
#endif //__EEPROM_H
