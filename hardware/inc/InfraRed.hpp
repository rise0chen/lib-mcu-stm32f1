#ifndef __INFRARED_H
#define __INFRARED_H
#include "sys.hpp"

//红外线
namespace InfraRed{
	extern Gpio *DQ;
	extern u32 IR_DATA;
	extern u8  IR_IRQ;
	
	void init(void);
	void get(void);
}
#endif //__INFRARED_H
