/*************************************************
Copyright (C), 2018-2028, Crise Tech. Co., Ltd.
File name: systick.cpp
Author: rise0chen
Version: 1.0
Date: 2019.08.30
Description: 滴答定时器
Usage:
  #include "systick.h"
  task.init(100); //初始化心跳时钟
History:
  rise0chen 2019.08.30   init
*************************************************/
#include "systick.h"

#define fac_us (9)              // us倍乘数 (rcc.clkSys/8 000 000)
#define fac_ms (fac_us * 1000)  // ms倍乘数
uint16_t sysTick_oneTime = 0;   // 1次系统心跳的时间(ms)
uint32_t sysTick_times = 0;     //系统心跳次数

/*************************************************
Function: tick_init
Description: 初始化SysTick心跳定时器
Input: nms 心跳时间(ms)
Return: void
*************************************************/
void sysTick_init(uint16_t nms) {
  sysTick_oneTime = nms;
  SysTick->CTRL = 0;             //复位
  SysTick->VAL = 0x00;           //清空
  SysTick->LOAD = nms * fac_ms;  // nms 1次系统心跳
  SysTick->CTRL = 3;             // 8分频 中断 使能

  task_init(nms);
}

/*************************************************
Function: SysTick_Handler
Description: 心跳定时器中断(每sysTick_oneTime毫秒触发1次)
Input: void
Return: void
*************************************************/
_C void SysTick_Handler(void) {
  sysTick_times++;
  task_tick();
}
