#ifndef __ADC_H
#define __ADC_H
#include "sys.hpp"

class Adc{
	public:
		Adc(uint8_t t);
		void init(void);
		uint16_t  get(uint8_t ch,uint8_t times=10);
		float getTemp(uint8_t times=10);
	private:
		ADC_TypeDef* the;
		uint32_t RCC_The;
		
};
extern Adc adc1;
extern Adc adc2;
extern Adc adc3;

#endif //__ADC_H
