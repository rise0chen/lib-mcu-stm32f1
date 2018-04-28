#ifndef __ADC_H
#define __ADC_H
#include "sys.hpp"

class Adc{
	public:
		ADC_TypeDef* the;
	
		Adc(u8 t);
		void init(void);
		u16  get(u8 ch,u8 times=10);
		float getTemp(u8 times=10);
	private:
		u32 RCC_The;
		
};
extern Adc adc1;
extern Adc adc2;
extern Adc adc3;
#endif //__ADC_H
