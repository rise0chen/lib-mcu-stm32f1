#ifndef __DAC_H
#define __DAC_H
#include "sys.hpp"

class dac{
	public:
		DAC_TypeDef* the;
	
		dac(u8 t);
		void init(void);
	private:
		u32 RCC_The;
		
};
extern dac dac1;
#endif //__DAC_H
