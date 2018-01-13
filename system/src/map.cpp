#include "map.hpp"

namespace map{
	void JTAG(u8 mode){
	//JTAG模式设置,用于设置JTAG的模式
	//mode:jtag,swd模式设置;00,全使能;01,使能SWD;10,全关闭;
	//#define JTAG_SWD_DISABLE 0X02
	//#define SWD_ENABLE	0X01
	//#define JTAG_SWD_ENABLE	0X00
		RCC->APB2ENR|= 1;//AF
		AFIO->MAPR &= 0XF8FFFFFF;//清除MAPR的[26:24]
		AFIO->MAPR |= (mode<<25);//设置jtag模式
	}
}
