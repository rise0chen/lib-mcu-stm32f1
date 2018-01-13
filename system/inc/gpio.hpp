#ifndef __GPIO_H
#define __GPIO_H
#include "sys.hpp"

//Ex_Config专用定义
#define FTI 1//下降沿触发
#define RTI 2//上升沿触发
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

class gpio{
  public:
    gpio(u8 x, u8 n);
    void  Config(GPIOMode_TypeDef Mode,s8 data=0,GPIOSpeed_TypeDef Speed=P_2MHz);
    void  Output(s8 data);
    u8    Input(void);
    void  ExConfig(u8 TRIM);//外部中断
    void  Lock(void);
    
    vu32* O;
    vu32* I;
  private:
    u8    Px;//GPIO
    GPIO_TypeDef* GPIOx;
    u8    Pn;//Pin
    u16   GPIOn;
};

#endif //__GPIO_H
