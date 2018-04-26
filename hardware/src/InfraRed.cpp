#include "InfraRed.hpp"

namespace InfraRed{
	gpio *DQ = &gpio_default;
	u32 IR_DATA; //定义一个32位数据变量，保存接收码
	u8  IR_IRQ;  //定义一个8位数据的变量，用于指示接收标志

	void init(void){
		DQ->config(P_UIN);
		DQ->configExti(FTI);//下降沿中断
	}
	u8 Tim(void){
		u8 t=0;
		while(*DQ->I){//高电平
			t++;
			delay_us(20);
			if(t>=250) return t;//超时溢出
		}
		return t;
	}
	void get(void){
		u32 data=0;
		u8 time=0,ok=0,d,num=0;
		
		while(1){
			if(*DQ->I){
				time=Tim();//获得此次高电平时间
				if(time>=250) break;//不是有用的信号
				if(time>=200 && time<250){
					ok=1;//收到起始信号
				}else if(time>=60 && time<90){
					d=1;//收到数据 1
				}else if(time>=10 && time<50){
					d=0;//收到数据 0
				}
				if(ok==1){
					data<<=1;
					data+=d;
					if(num>=32){
						IR_DATA=data;
						IR_IRQ=1;//接收完毕，产生IRQ标记
						break;
					}
				}
				num++;
			}
		}
	}
}
/*
_C void EXTI15_10_IRQHandler(void){
	if(EXTI->PR & (1<<15)){//红外遥控外部中断
		InfraRed::Get();
		if(InfraRed::IR_IRQ==1){
			InfraRed::IR_IRQ=0;
			//InfraRed::IR_DATA;
		}
		EXTI->PR=1<<15;
	}
}
*/
