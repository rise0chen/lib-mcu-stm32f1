#ifndef __TIM_H
#define __TIM_H
#include "sys.hpp"

class tim{
	public:
		TIM_TypeDef* the;
		
		tim(u8 t);
		void config(u16 div, u16 period, u8 TI=0);
		void configOC(u8 channel, u16 dutyFactor, u8 dTime=0);
		void configIC(u16 channel, u16 ICPSC_DIVx);
		void configBD(u8 dTime);
		void cmd(FunctionalState state);
		void clear();
	private:
		u8 IRQn;

};
extern tim tim1;//high
extern tim tim2;
extern tim tim3;
extern tim tim4;
#ifdef STM32F10X_HD
extern tim tim5;
extern tim tim6;//base
extern tim tim7;//base
extern tim tim8;//high
#endif //STM32F10X_HD 

#endif //__TIM_H
