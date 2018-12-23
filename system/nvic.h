#ifndef __NVIC_H
#define __NVIC_H
#include "sys.h"

void nvic_setVectorTable(u32 vectTab, u32 offset);//设置偏移地址
void nvic_configGroup(u8 group);//设置NVIC分组
void nvic_config(u8 channel, u8 preemptionPriority, u8 subPriority=3);//设置中断
void nvic_disable(void);//关闭所有中断
void nvic_enable(void);//开启所有中断

#endif //__NVIC_H
