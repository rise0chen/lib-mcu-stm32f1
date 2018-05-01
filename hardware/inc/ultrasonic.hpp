#ifndef __ULTRASONIC_H
#define __ULTRASONIC_H
#include "sys.hpp"

class Ultrasonic{
	public:
		u32 distance;//单位毫米mm
	
		Ultrasonic(Gpio *TRIG, Gpio *ECHO);
		u32  ranging(u8 num=1);
	
	private:
		Gpio *TRIG;
		Gpio *ECHO;
};
#endif //__TEMPLATE_H
