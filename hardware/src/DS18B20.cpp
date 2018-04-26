#include "DS18B20.hpp"

namespace DS18B20{
	gpio *DQ = &gpio_default;
	
	void init(void){
		DQ->config(P_PPO);
		*DQ->O=1;
		delay_us(10);
		*DQ->O=0;
		delay_us(600);
		*DQ->O=1;
		delay_us(50);
		*DQ->O=0;
		delay_us(200);
		*DQ->O=1;
		delay_us(400);
	}
	void write(u8 dat){
		u8 i;
		DQ->config(P_PPO);
		*DQ->O=1;
		delay_us(10);
		for(i=0;i<8;i++){
			*DQ->O=0;
			if(dat&0x01){
				*DQ->O=1;
			}else
				*DQ->O=0;
			dat>>=1;
			delay_us(60);
			*DQ->O=1;
			delay_us(10);
		}
		*DQ->O=1;
		delay_us(10);
	}
	u8 read(){
		u8 i,value=0;//一定要给value附初值否则显示会错误
		DQ->config(P_PPO);
		*DQ->O=1;
		for(i=0;i<8;i++){
			*DQ->O=0;
			value>>=1;
			*DQ->O=1;
			DQ->config(P_FIN);
			if(*DQ->I==1){
				value|=0x80;
			}
			delay_us(50);
			DQ->config(P_PPO);
			*DQ->O=1;
			delay_us(1);
		}
		*DQ->O=1;
		delay_us(10);
		return value;
	}
	u8 getTemp(){//读取温度内需要复位的
		u8 a,b;
		
		init();//初始化
		write(0xcc);//发送忽略ROM指令
		write(0x44);//发送温度转换指令
		delay_us(1000);
		init();//初始化
		write(0xcc);//发送忽略ROM指令
		write(0xbe);//发读暂存器指令
		a=read();//温度的低八位
		b=read();//温度的高八位
		b<<=4;//ssss s***；s为标志位s=0表示温度值为正数，s=1温度值为负数
		b+=(a&0xf0)>>4;
		a=a&0x0f;//温度的小数部分
		return b;
	}
}
