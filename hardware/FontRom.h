#ifndef __FONTROM_H
#define __FONTROM_H
#include "sys.h"
#include "spi.h"

typedef struct {
		SpiStruct *com;
		GpioStruct *CS;
} FontRomStruct;  //字库结构

FontRomStruct* fontRom_new(SpiStruct *spi_com, GpioStruct *spi_cs);
void fontRom_init(FontRomStruct* self);
void fontRom_getWord16x16(FontRomStruct* self, uint16_t gbk, uint8_t* data);
void fontRom_getWord8x16(FontRomStruct* self, uint8_t ascii, uint8_t* data);
unsigned char fontRom_utf8ToGbk(FontRomStruct* self, const char* bufIn, char* bufOut);
	

#endif /* __FONTS_H */
