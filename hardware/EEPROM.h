#ifndef __EEPROM_H
#define __EEPROM_H
#include "i2c.h"

typedef struct {
		uint8_t  pageSize;
		uint16_t pageMax;
  
		I2cStruct* com;
		uint8_t  deviceAddr;
		uint8_t  type;
} EepromStruct;  //Eeprom结构

EepromStruct* eeprom_new(I2cStruct* com, uint8_t addr, uint8_t type);
ErrorStatus  eeprom_write(EepromStruct* self, uint16_t addr,void* buf,uint16_t num);
ErrorStatus  eeprom_read(EepromStruct* self, uint16_t addr,void* buf,uint16_t num);

#endif //__EEPROM_H
