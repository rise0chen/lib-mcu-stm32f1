#ifndef __TIM_H
#define __TIM_H
#include "sys.hpp"

class Tim{
	public:
		TIM_TypeDef* the;
		
		Tim(u8 t);
		void config(u16 div, u16 period, u8 TI=0);
		void configOC(u8 channel, u16 dutyFactor, u8 dTime=0);
		void configIC(u16 channel, u16 div);
		void configBD(u8 dTime);
		void cmd(FunctionalState state);
		void clear();
	private:
		u8 IRQn;

};
extern Tim tim1;//high
extern Tim tim2;
extern Tim tim3;
extern Tim tim4;
#ifdef STM32F10X_HD
extern Tim tim5;
extern Tim tim6;//base
extern Tim tim7;//base
extern Tim tim8;//high
#endif //STM32F10X_HD 

#endif //__TIM_H
