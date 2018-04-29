#ifndef __FONTROM_H
#define __FONTROM_H
#include "sys.hpp"
#include "spi.hpp"

class FontRom{
	public:
		void init(Spi  *spi_com, Gpio *spi_cs);
		void getWord16x16(u16 gbk, u8* data);
		void getWord8x16(u8 ascii, u8* data);
		u8 utf8ToGbk(const char* bufIn, char* bufOut);
	private:
		Spi *com = &spi2;
		Gpio *CS = &gpio_default;
};

#endif /* __FONTS_H */
