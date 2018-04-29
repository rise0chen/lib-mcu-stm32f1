#ifndef __INFRARED_H
#define __INFRARED_H
#include "sys.hpp"

//红外线
class InfraRed{
	public:
		u32 IR_DATA;//定义一个32位数据变量，保存接收码
		u8  IR_IRQ; //定义一个8位数据的变量，用于指示接收标志
		
		void init(Gpio *P_DQ);
		void get(void);
	private:
		Gpio *DQ = &gpio_default;
		u8 tim(void);
};
extern InfraRed infraRed;

#endif //__INFRARED_H
