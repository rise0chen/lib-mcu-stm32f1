#ifndef __FONTROM_H
#define __FONTROM_H
#include "sys.h"
#include "spi.hpp"

class FontRom{
	public:
		FontRom(Spi *spi_com, Gpio *spi_cs);
		void init(void);
		void getWord16x16(u16 gbk, u8* data);
		void getWord8x16(u8 ascii, u8* data);
		unsigned char utf8ToGbk(const char* bufIn, char* bufOut);
	
	private:
		Spi *com;
		Gpio *CS;
};

#endif /* __FONTS_H */
