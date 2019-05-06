#ifndef __DAC_H
#define __DAC_H
#include "sys.hpp"

class Dac{
	public:
		Dac(uint8_t t);
		void init(void);
	private:
		DAC_TypeDef* the;
		uint32_t RCC_The;
		
};
extern Dac dac1;

#endif //__DAC_H
