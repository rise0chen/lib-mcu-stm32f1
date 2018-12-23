#ifndef __ULTRASONIC_H
#define __ULTRASONIC_H
#include "sys.h"

class Ultrasonic{
	public:
		u16 distance;//单位毫米mm
	
		Ultrasonic(Gpio *TRIG, Gpio *ECHO);
		u16  get(u8 num=1);
	
	private:
		Gpio *TRIG;
		Gpio *ECHO;
};
#endif //__TEMPLATE_H
