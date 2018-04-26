#include "pwr.hpp"

namespace pwr{
	void WFI_SET(void){
		__wfi();
		__ASM volatile("wfi");
	}
	void reset(void){//系统软复位
		SCB->AIRCR =0X05FA0000|0x04;
	}
	void sleep(u8 next){//进入睡眠模式(任意中断唤醒)
		SCB->SCR &= ~(1<<2);//清除SLEEPDEEP位
		if(next==1){
			SCB->SCR |= 1<<1;//使能SLEEPONEXIT位(中断执行后,继续休眠)
		}else{
			SCB->SCR &= ~(1<<1);//清除SLEEPONEXIT位(中断执行后,退出休眠)
		}
		WFI_SET();//执行WFI指令
	}
	void stop(void){//进入停止模式(外部中断唤醒)
		SysTick->CTRL = 0;//关闭SysTick
		DEM_CR &= ~(1<<24);//关闭DWT等
		SCB->SCR |= 1<<2;//使能SLEEPDEEP位 (SYS->CTRL)
		PWR->CR &= ~(1<<1);//清除PDDS位
		PWR->CR |= 1<<0;//LPDS置位（唤醒时间长，但低功耗）
		WFI_SET();//执行WFI指令
	}
	void standby(void){//进入待机模式(PA0上升沿唤醒)
		SCB->SCR |= 1<<2;//使能SLEEPDEEP位 (SYS->CTRL)
		RCC->APB1ENR |= 1<<28;//使能电源时钟
		PWR->CSR |= 1<<8;//WKUP(PA0上升沿唤醒)
		PWR->CR |= 1<<2;//清除Wake-up 标志
		PWR->CR |= 1<<1;//PDDS置位
		WFI_SET();//执行WFI指令,进入待机模式
	}
}
