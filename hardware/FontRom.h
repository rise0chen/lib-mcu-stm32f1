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
void fontRom_getWord16x16(FontRomStruct* self, u16 gbk, u8* data);
void fontRom_getWord8x16(FontRomStruct* self, u8 ascii, u8* data);
unsigned char fontRom_utf8ToGbk(FontRomStruct* self, const char* bufIn, char* bufOut);
	

#endif /* __FONTS_H */
