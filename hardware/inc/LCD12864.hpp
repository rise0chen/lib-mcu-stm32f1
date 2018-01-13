#ifndef __LCD12864_H
#define __LCD12864_H
#include "sys.hpp"
#include "font.hpp"

namespace LCD12864{
	extern u8 STA;
	
	void Init(void);
	void WriteCmd(u8 data);
	void WriteData(u8 data);
	void Reset(void);
	void Open(void);
	void Close(void);
	void Clear(void);
	void SetAddr(u8 page,u8 column);
	void DisplayFull(u8 data1,u8 data2);
	void DisplayGBKString(u8 page,u8 column,u8 reverse,char* text);
	void Display5x8String(u8 page,u8 column,u8 reverse,char* text);
	void Display8x16Char(u8 page,u8 column,u8 ASCII_CODE);
}
#endif //__LCD12864_H
