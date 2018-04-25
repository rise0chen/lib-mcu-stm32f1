#ifndef __INFRARED_H
#define __INFRARED_H
#include "sys.hpp"

//红外线
namespace InfraRed{
	extern gpio *DQ;
	extern u32 IR_DATA;
	extern u8  IR_IRQ;
	
	void Init(void);
	void Get(void);
}
#endif //__INFRARED_H
