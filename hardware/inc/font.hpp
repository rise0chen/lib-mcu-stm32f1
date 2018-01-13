#ifndef __FONTS_H
#define __FONTS_H
#include "sys.hpp"

#define UseFontRom		1		//I2C软件操控
extern char AsciiTable8x16[95][16];
extern char AsciiTable5x8[95][5];

#if UseFontRom
#include "spi.hpp"
#include "LCD12864.hpp"
namespace FontRom{
	extern gpio CS;
	u8 convert(u8 data);
	void ReadWrite16x16(u32 fontaddr,u8 reverse,u8 page,u8 column);
	void ReadWrite8x16(u32 fontaddr,u8 reverse,u8 page,u8 column);
	void ReadWrite5x8(u32 fontaddr,u8 reverse,u8 page,u8 column);
}
#endif

#endif /* __FONTS_H */
