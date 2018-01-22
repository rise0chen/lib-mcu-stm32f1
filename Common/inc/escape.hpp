#ifndef __ESCAPE_H
#define __ESCAPE_H
#include "std.hpp"

typedef struct{
	unsigned char len[2];
	unsigned char buf[2][8];
}ESC_TypeDef;

unsigned short Escape(char* bufin, unsigned short lenin, ESC_TypeDef* type, unsigned char typelen, unsigned char way);

#endif //__ESCAPE_H
