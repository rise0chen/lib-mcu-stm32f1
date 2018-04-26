#ifndef __ULTRASONIC_H
#define __ULTRASONIC_H
#include "sys.hpp"

namespace ultrasonic{
	extern gpio *TRIG;
	extern gpio *ECHO;
	extern u32 distance;//单位毫米mm
	
	void init(void);
	void ranging(u8 num=1);
}
#endif //__TEMPLATE_H
