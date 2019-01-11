#ifndef __NVIC_H
#define __NVIC_H
#include "sys.h"

void nvic_setVectorTable(uint32_t vectTab, uint32_t offset);//设置偏移地址
void nvic_configGroup(uint8_t group);//设置NVIC分组
void nvic_config(uint8_t channel, uint8_t preemptionPriority, uint8_t subPriority);//设置中断
void nvic_disable(void);//关闭所有中断
void nvic_enable(void);//开启所有中断

#endif //__NVIC_H
