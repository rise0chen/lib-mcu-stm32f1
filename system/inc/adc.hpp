#ifndef __ADC_H
#define __ADC_H
#include "sys.hpp"

class adc{
	public:
		ADC_TypeDef* the;
	
		adc(u8 t);
		void init(void);
		u16  get(u8 ch,u8 times=10);
		float getTemp(u8 times=10);
	private:
		u32 RCC_The;
		
};
extern adc adc1;
extern adc adc2;
extern adc adc3;
#endif //__ADC_H
