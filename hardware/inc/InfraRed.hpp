#ifndef __INFRARED_H
#define __INFRARED_H
#include "sys.hpp"

//红外线
class InfraRed{
	public:
		u32 DATA;//定义一个32位数据变量，保存接收码
		u8  IRQ; //定义一个8位数据的变量，用于指示接收标志
		
		InfraRed(Gpio *P_DQ);
		void get(void);
	private:
		Gpio *DQ;
		u8 tim(void);
};

#endif //__INFRARED_H
