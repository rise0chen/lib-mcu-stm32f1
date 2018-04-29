#ifndef __ULTRASONIC_H
#define __ULTRASONIC_H
#include "sys.hpp"

class Ultrasonic{
	public:
		u32 distance;//单位毫米mm
	
		void init(Gpio *TRIG, Gpio *ECHO);
		u32  ranging(u8 num=1);
	
	private:
		Gpio *TRIG = &gpio_default;
		Gpio *ECHO = &gpio_default;
};
#endif //__TEMPLATE_H
