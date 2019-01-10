#ifndef __EEPROM_H
#define __EEPROM_H
#include "i2c.h"

typedef struct {
		u8  pageSize;
		u16 pageMax;
  
		I2cStruct* com;
		u8  deviceAddr;
		u8  type;
} EepromStruct;  //Eeprom结构

EepromStruct* eeprom_new(I2cStruct* com, u8 addr, u8 type);
ErrorStatus  eeprom_write(EepromStruct* self, u16 addr,void* buf,u16 num);
ErrorStatus  eeprom_read(EepromStruct* self, u16 addr,void* buf,u16 num);

#endif //__EEPROM_H
