#ifndef __GPIO_H
#define __GPIO_H
#include "sys.h"

//Ex_Config专用定义
#define FTI 1 //下降沿触发
#define RTI 2 //上升沿触发
#define ATI 3 //任意电平触发

//GPIO定义
#define PA    0
#define PB    1
#define PC    2
#define PD    3
#define PE    4
#define PF    5
#define PG    6
#define P0    (0x0001)
#define P1    (0x0002)
#define P2    (0x0004)
#define P3    (0x0008)
#define P4    (0x0010)
#define P5    (0x0020)
#define P6    (0x0040)
#define P7    (0x0080)
#define PLow  (0x00FF)
#define P8    (0x0100)
#define P9    (0x0200)
#define P10   (0x0400)
#define P11   (0x0800)
#define P12   (0x1000)
#define P13   (0x2000)
#define P14   (0x4000)
#define P15   (0x8000)
#define PHigh (0xFF00)
#define PAll  (0xFFFF)

typedef enum{
	P_10MHz = 1,
	P_2MHz,
	P_50MHz
}GPIOSpeed_TypeDef;
typedef enum{
	P_AIN  = 0x00,//模拟输入
	P_FIN  = 0x04,//浮空输入
	P_DIN  = 0x28,//下拉输入
	P_UIN  = 0x48,//上拉输入

	P_ODO  = 0x14,//开漏输出
	P_PPO  = 0x10,//推挽输出
	P_ODAF = 0x1C,//复用开漏输出
	P_PPAF = 0x18,//复用推挽输出

	P_EVO  = 0x80//EVENTOUT,用多个ARM间的时钟同步。执行SEV指令 或 __SEV()时，产生几个机器周期的高电平。
}GPIOMode_TypeDef;

class Gpio{
	public:
		vu32* O;
		vu32* I;
		
		Gpio(){};
		Gpio(u8 x, u8 n);
		void  config(GPIOMode_TypeDef mode,s8 data=0,GPIOSpeed_TypeDef speed=P_2MHz);
		void  output(s8 data);
		u8    input(void);
		void  configExti(u8 TRIM);//外部中断
		void  lock(void);

	private:
		GPIO_TypeDef* GPIOx;
		u8    Px;//GPIO
		u8    Pn;//Pin
};
extern Gpio gpio_default;

#endif //__GPIO_H
