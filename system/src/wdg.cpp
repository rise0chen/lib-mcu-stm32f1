#include "wdg.hpp"

void IWDG_Config(u8 prer,u16 rlr){//独立看门狗初始化配置
//初始化独立看门狗
//例：IWDG_Config(6,1250);//8s
//prer:分频数:0~6(只有低3位有效!)
//分频因子=4*2^prer.但最大值只能是256!
//rlr:重装载寄存器值:低11位有效.
//时间计算(大概):Tout=((4*2^prer)*rlr)/40 (ms).
	IWDG->KR=0X5555;//使能对IWDG->PR和IWDG->RLR的写
	IWDG->PR=prer;//设置分频系数
	IWDG->RLR=rlr;//重装载寄存器 IWDG->RLR
	IWDG->KR=0XAAAA;//reload
	IWDG->KR=0XCCCC;//使能看门狗
}
void IWDG_Feed(void){//喂独立看门狗
	IWDG->KR=0XAAAA;//reload
}

void WWDG_Config(u8 tr,u8 wr,u8 fprer){
//初始化窗口看门狗
//例：WWDG_Config(0X7F,0X5F,3);
//tr	 :T[6:0],计数器值
//wr	 :W[6:0],窗口值
//fprer:分频系数（WDGTB）,仅最低2位有效
//Fwwdg=PCLK1/(4096*2^fprer).
	rcc::Cmd(1, APB1_WWDG, ENABLE);//使能wwdg时钟
	WWDG->CFR|=fprer<<7;//PCLK1/4096再除2^fprer
	WWDG->CFR&=0XFF80;
	WWDG->CFR|=wr;//设定窗口值
	WWDG->CR|=tr&0x7F;//设定计数器值
	WWDG->CR|=1<<7;//开启看门狗
	nvic::Init(WWDG_IRQn,3,3);//抢占3，子优先级3，组2
	WWDG->SR=0X00;//清除提前唤醒中断标志位
	WWDG->CFR|=1<<9;//使能提前唤醒中断
}

_C void WWDG_IRQHandler(void){//窗口看门狗中断程序
	WWDG->CR =(0x7F);//重设窗口看门狗的值!
	WWDG->SR=0X00;//清除提前唤醒中断标志位
	gpio(PC,13).Output(-1);
}
