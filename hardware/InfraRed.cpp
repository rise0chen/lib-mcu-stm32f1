/*************************************************
Copyright (C), 2018-2028, Crise Tech. Co., Ltd.
File name: InfraRed.cpp
Author: rise0chen
Version: 1.0
Date: 2018.4.26
Description: 红外通信
Usage:
        #include "InfraRed.h"
        InfraRed ir(&gpio_default);
        if(EXTI->PR & (1<<15)){//红外遥控外部中断
                ir.get();
                if(ir.IRQ==1){
                        ir.IRQ=0;
                        //ir.DATA;
                }
                EXTI->PR=1<<15;
        }
History:
        rise0chen   2018.4.26   编写注释
*************************************************/
#include "InfraRed.h"

/*************************************************
Function: InfraRed::InfraRed
Description: InfraRed构造函数
Input:
        P_DQ  红外接收引脚
Return: InfraRed类
*************************************************/
InfraRed::InfraRed(Gpio* P_DQ) {
  DQ = P_DQ;
}

/*************************************************
Function: InfraRed::tim
Description: 信号计时
Input: void
Return: 20us的个数
*************************************************/
uint8_t InfraRed::tim(void) {
  uint8_t t = 0;
  while (*DQ->I) {  //高电平
    t++;
    delay_us(20);
    if (t >= 250)
      return t;  //超时溢出
  }
  return t;
}

/*************************************************
Function: InfraRed::get
Description: 读取红外信号
Input: void
Return: void
*************************************************/
void InfraRed::get(void) {
  uint32_t data = 0;
  uint8_t time = 0, ok = 0, d = 0, num = 0;

  DQ->config(P_UIN);
  DQ->configExti(FTI);  //下降沿中断
  while (1) {
    if (*DQ->I) {
      time = tim();  //获得此次高电平时间
      if (time >= 250)
        break;  //不是有用的信号
      if (time >= 200 && time < 250) {
        ok = 1;  //收到起始信号
      } else if (time >= 60 && time < 90) {
        d = 1;  //收到数据 1
      } else if (time >= 10 && time < 50) {
        d = 0;  //收到数据 0
      }
      if (ok == 1) {
        data <<= 1;
        data += d;
        if (num >= 32) {
          DATA = data;
          IRQ = 1;  //接收完毕，产生IRQ标记
          break;
        }
      }
      num++;
    }
  }
}
