#include "ultrasonic.hpp"

namespace ultrasonic{
	gpio &TRIG=gpio_default;
	gpio &ECHO=gpio_default;
	u32 distance;
	
	void Init(void){
		TRIG.Config(P_PPO);
		ECHO.Config(P_UIN);
	}
	void Ranging(u8 num){
		u8 i;
		u16 j=0;
		u32 Ultr_Temp=0;
		for(i=0;i<num;i++){
			*TRIG.O=1;
			delay_ms(10);
			*TRIG.O=0;
			while((!*ECHO.I));
			while(*ECHO.I){
				delay_us(10);
				j++;
			}
			Ultr_Temp+=340/2*j*10;//模块最大可测距3m 
			j=0;
			delay_ms(60);//防止发射信号对回响信号的影响
		}
		distance=Ultr_Temp/num/1000;
	}
}
