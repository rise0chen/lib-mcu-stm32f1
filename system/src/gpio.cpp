#include "gpio.hpp"

gpio gpio_default(PC, 13);

gpio::gpio(u8 x, u8 n){
	Px = x;
	GPIOx = (GPIO_TypeDef*)(GPIOA_BASE+(x<<10));
	Pn = n;
	O  = BITBAND((int)&GPIOx->ODR, n);
	I  = BITBAND((int)&GPIOx->IDR, n);
}
void gpio::config(GPIOMode_TypeDef mode, s8 data, GPIOSpeed_TypeDef speed){
	uint32_t currentmode = 0x00, pos = 0x00;
	uint32_t tmpreg = 0x00;

	RCC->APB2ENR |= 1<<(Px+2);//时钟
	if((mode & 0x80) == 0){
		currentmode = mode & 0x0F;
		if((mode & 0x10) != 0)
			currentmode |= speed;
		if(Pn<8){
			tmpreg = GPIOx->CRL;
			pos = Pn << 2;
			tmpreg &= ~((0x0F) << pos);//clear
			tmpreg |= (currentmode << pos);//revise
			GPIOx->CRL = tmpreg;//write
		}else{
			tmpreg = GPIOx->CRH;
			pos = (Pn-8) << 2;
			tmpreg &= ~((0x0F) << pos);//clear
			tmpreg |= (currentmode << pos);//revise
			GPIOx->CRH = tmpreg;//write
		}
		if(mode == P_DIN)
			GPIOx->BRR = (1<<Pn);
		if(mode == P_UIN)
			GPIOx->BSRR = (1<<Pn);
		if((mode & 0x10) != 0)
			*this->O = data;
	}else{
		switch((u8)mode){
			case 0x80:
				AFIO->EVCR =(1<<7) | (Px << 0x04) | Pn;
			break;
		}
	}
}
void gpio::output(s8 data){
	switch(data){
		case 1 :GPIOx->BSRR = (1<<Pn);break;//置1
		case 0 :GPIOx->BRR  = (1<<Pn);break;//置0
		case -1:(GPIOx->ODR & (1<<Pn)) ? (GPIOx->BRR = (1<<Pn)) : (GPIOx->BSRR = (1<<Pn));break;//翻转
	}
}
u8 gpio::input(){
	return (GPIOx->IDR & (1<<Pn)) ? 1 : 0;
}
void gpio::configExti(u8 TRIM){
//外部中断配置函数
//只针对GPIOA~G;不包括PVD,RTC和USB唤醒这三个
//参数:
//TRIM:触发模式,1,下升沿;2,上降沿;3，任意电平触发
//该函数一次只能配置1个IO口,多个IO口,需多次调用
//该函数会自动开启对应中断,以及屏蔽线
	u8 EXTADDR, EXTOFFSET, IRQn;

	EXTADDR  = (Pn>>2);//得到中断寄存器组的编号
	EXTOFFSET = (Pn%4)<<2;
	RCC->APB2ENR |= 0x01;//使能io复用时钟
	AFIO->EXTICR[EXTADDR] &= ~(0x000F<<EXTOFFSET);//clear
	AFIO->EXTICR[EXTADDR] |= Px<<EXTOFFSET;//write
//自动设置
	EXTI->IMR |= (1<<Pn);//开启linePn上的中断
//EXTI->EMR|=1<<Pn;//不屏蔽linePn上的事件(如果不屏蔽这句,在硬件上是可以的,但是在软件仿真的时候无法进入中断!)
	if(TRIM&0x01) EXTI->FTSR |= 1<<Pn;//linePn上事件下降沿触发
	if(TRIM&0x02) EXTI->RTSR |= 1<<Pn;//linePn上事件上升降沿触发
	if(Pn<5){
		IRQn = 6+Pn;
	}else if(Pn>=5 && Pn<10){
		IRQn = 23;
	}else if(Pn>=10 && Pn<16){
		IRQn = 40;
	}
	nvic::init(IRQn,1,0);
}
void gpio::lock(){
	u32 tmp = 0x00010000;
	tmp |= (1<<Pn);
	GPIOx->LCKR = tmp;//1
	GPIOx->LCKR = (1<<Pn);//0
	GPIOx->LCKR = tmp;//1
	tmp = GPIOx->LCKR;
	tmp = GPIOx->LCKR;
}
