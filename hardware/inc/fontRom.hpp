#ifndef __FONTROM_H
#define __FONTROM_H
#include "sys.hpp"
#include "spi.hpp"

namespace FontRom{
	extern gpio &CS;
	extern spi &com;
	
	void getWord16x16(u16 gbk, u8* data);
	void getWord8x16(u8 ascii, u8* data);
}

#endif /* __FONTS_H */
