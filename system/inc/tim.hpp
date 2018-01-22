#ifndef __TIM_H
#define __TIM_H
#include "sys.hpp"

class tim{
	public:
		TIM_TypeDef* the;
		
		tim(u8 t);
		void BaseConfig(u16 div, u16 Period, u8 TI=0);
		void OCConfig(u8 Pin, u16 DutyFactor, u8 DTime=0);
		void ICConfig(u16 Channel_x, u16 ICPSC_DIVx);
		void BDConfig(u8 DTime);
		void Cmd(FunctionalState state);
		void Clear();
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
