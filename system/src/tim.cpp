#include "tim.hpp"

tim tim1 = tim(1);//high
tim tim2 = tim(2);
tim tim3 = tim(3);
tim tim4 = tim(4);
#ifdef STM32F10X_HD
tim tim5 = tim(5);
tim tim6 = tim(6);//base
tim tim7 = tim(7);//base
tim tim8 = tim(8);//high
#endif //STM32F10X_HD

tim::tim(u8 t){
	switch(t){
		case 1:
			the=TIM1;
			IRQn=TIM1_UP_IRQn;
			break;
		case 2:
			the=TIM2;
			IRQn=TIM2_IRQn;
			break;
		case 3:
			the=TIM3;
			IRQn=TIM3_IRQn;
			break;
		case 4:
			the=TIM4;
			IRQn=TIM4_IRQn;
			break;
		#ifdef STM32F10X_HD
		case 5:
			the=TIM5;
			IRQn=TIM5_IRQn;
			break;
		case 6:
			the=TIM6;
			IRQn=TIM6_IRQn;
			break;
		case 7:
			the=TIM7;
			IRQn=TIM7_IRQn;
			break;
		case 8:
			the=TIM8;
			IRQn=TIM8_UP_IRQn;
			break;
		#endif //STM32F10X_HD
	}
}
void	tim::BaseConfig(u16 div, u16 Period, u8 TI){
	the->PSC = div -1;//分频
	the->ARR = Period -1;//周期
	the->CR1 = 0x0004;//向上计数等...
//	if ((the == TIM1) || (the == TIM8)){
//		the->RCR = 0;//重复计数
//	}
	if(TI){
		nvic::Init(IRQn, 2, 0);
		the->SR	 = 0;//清除中断
		the->DIER = 1;//溢出中断
	}
	Cmd(ENABLE);
}
void	tim::OCConfig(u8 Channel, u16 DutyFactor, u8 DTime){
	uint16_t tmpccmrx = 0, tmpccer = 0, tmpcr2 = 0;

	the->CCER &= (~(uint16_t)(1<<(4*(Channel-1))));//禁止引脚输出
	tmpccer = the->CCER;
	tmpcr2 =	the->CR2;
	
	if((the == TIM1) || (the == TIM8)){
		if(DTime!=0){
			tmpccer &= (uint16_t)(~((uint16_t)(8<<(4*(Channel-1)))));
			tmpccer |= (uint16_t)(4<<(4*(Channel-1)));//使能互补输出
			tmpcr2 &= (uint16_t)(~((uint16_t)0x0100<<(2*(Channel-1))));//死区低电平
			tmpcr2 &= (uint16_t)(~((uint16_t)0x0200<<(2*(Channel-1))));//死区低电平
			the->BDTR &= ~0xFF;
			the->BDTR |= (0x8D00+DTime);//输出使能,死区时间DTime/SYSTEMCLK
		}
	}
	tmpccer &= (~((uint16_t)(1<<(4*(Channel-1)))<<1));//高电平有效
	tmpccer |= (1<<(4*(Channel-1)));//使能引脚输出
	
	the->CCER = tmpccer;
	the->CR2 = tmpcr2;
	
	if(Channel==1 || Channel==2){
		tmpccmrx = the->CCMR1;
		tmpccmrx &= (~((uint16_t)0x73<<(8*(Channel-1))));
		tmpccmrx |= 0x60<<(8*(Channel-1));//PWM1
		the->CCMR1 = tmpccmrx;
	}else{
		tmpccmrx = the->CCMR2;
		tmpccmrx &= (~((uint16_t)0x73<<(8*(Channel-3))));
		tmpccmrx |= 0x60<<(8*(Channel-3));//PWM1
		the->CCMR2 = tmpccmrx;
	}
	switch(Channel){//占空比
		case 1:the->CCR1 = (u16)((the->ARR + 1)/100*DutyFactor);break;
		case 2:the->CCR2 = (u16)((the->ARR + 1)/100*DutyFactor);break;
		case 3:the->CCR3 = (u16)((the->ARR + 1)/100*DutyFactor);break;
		case 4:the->CCR4 = (u16)((the->ARR + 1)/100*DutyFactor);break;
	}
}
void	tim::ICConfig(u16 Channel_x, u16 ICPSC_DIVx){

}
void	tim::BDConfig(u8 DTime){

}
void	tim::Cmd(FunctionalState state){
	if(state){
		the->EGR |= 1;//计数器归零
		the->CNT = 0;//计数器归零
		the->CR1 |= 1;//使能
	}else{
		the->CR1 &= ~1;//失能
	}
}
void	tim::Clear(){
	the->EGR |= 1;//计数器归零
	the->CNT = 0;//计数器归零
}
