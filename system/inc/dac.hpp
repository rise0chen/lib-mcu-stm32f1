#ifndef __DAC_H
#define __DAC_H
#include "sys.hpp"

class Dac{
	public:
		Dac(u8 t);
		void init(void);
	private:
		DAC_TypeDef* the;
		u32 RCC_The;
		
};
extern Dac dac1;

#endif //__DAC_H
