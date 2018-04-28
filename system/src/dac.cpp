#include "Dac.hpp"

Dac dac1(1);

Dac::Dac(u8 t){
	switch(t){
		case 1:
			//the=DAC1;
			//RCC_The=APB1_DAC1;
			break;
	}
}
void Dac::init(void){
	
}
