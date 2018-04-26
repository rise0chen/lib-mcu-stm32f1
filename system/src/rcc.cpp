#include "rcc.hpp"

uint32_t SystemCoreClock;
u32 sysclk;
u32 ahbclk;
u32 apb1clk;
u32 apb2clk;
namespace rcc{
	void cmd(u8 bus, u32 periph, FunctionalState state){
		switch(bus){
			case 0:
				state ? RCC->AHBENR |=periph : RCC->AHBENR &=~periph;
			break;
			case 1:
				state ? RCC->APB1ENR|=periph : RCC->APB1ENR&=~periph;
			break;
			case 2:
				state ? RCC->APB2ENR|=periph : RCC->APB2ENR&=~periph;
			break;
		}
	}
	void reset(u8 bus, u32 periph){
		switch(bus){
			case 1:
				RCC->APB1RSTR |= periph;
				RCC->APB1RSTR &= ~periph;
			break;
			case 2:
				RCC->APB2RSTR |= periph;
				RCC->APB2RSTR &= ~periph;
			break;
		}
	}

	static void DeInit(void){
	//不能在这里执行所有外设复位!否则至少引起串口不工作.
	//把所有时钟寄存器复位
		RCC->APB1RSTR= 0x00000000;//复位结束
		RCC->APB2RSTR= 0x00000000;

		RCC->AHBENR  = 0x00000014;//睡眠模式闪存和SRAM时钟使能.其他关闭
		RCC->APB2ENR = 0x00000000;//外设时钟关闭
		RCC->APB1ENR = 0x00000000;//外设时钟关闭
		RCC->CR   |= 0x00000001;//使能内部高速时钟HSION
		RCC->CFGR &= 0xF0FF0000;//复位SW[1:0],HPRE[3:0],PPRE1[2:0],PPRE2[2:0],ADCPRE[1:0],MCO[2:0]
		RCC->CR   &= 0xFEF6FFFF;//复位HSEON,CSSON,PLLON
		RCC->CR   &= 0xFFFBFFFF;//复位HSEBYP
		RCC->CFGR &= 0xFF80FFFF;//复位PLLSRC,PLLXTPRE,PLLMUL[3:0]andUSBPRE
		RCC->CIR = 0x00000000;//关闭所有中断
	//配置向量表
	#ifdef VECT_TAB_RAM
		nvic::setVectorTable(SRAM_BASE,0x0);
	#else
		nvic::setVectorTable(FLASH_BASE,0x0);
	#endif
	}
	void init(u8 PLL){
	//系统时钟初始化函数
	//pll:选择的倍频数，从2开始，最大值为16
		DeInit();//复位并配置向量表
		RCC->CR   |= (1<<16);//HSE使能
		while(!(RCC->CR & (1<<17)));//等待HSE就绪
		RCC->CFGR=0X00000400;//APB1=DIV2;APB2=DIV1;AHB=DIV1;
		PLL -= 2;//抵消2个单位（因为是从2开始的，设置0就是2）
		RCC->CFGR |= (PLL<<18);//设置PLL值2~16
		RCC->CFGR |= (1<<16);//PLL输入始终源 HSE
		FLASH->ACR|= 0x32;//FLASH2个延时周期//0x12
		RCC->CR   |= (1<<24);//PLL使能
		while(!(RCC->CR & (1<<25)));//等待PLL就绪
		RCC->CFGR |= 2;//PLL作为系统时钟
		while(!(RCC->CFGR & (1<<3)));//等待PLL作为系统时钟设置成功
		SystemCoreClock=72000000;
		sysclk  = SystemCoreClock;
		ahbclk  = sysclk;
		apb1clk = ahbclk>>1;
		apb2clk = ahbclk;
	}
}
