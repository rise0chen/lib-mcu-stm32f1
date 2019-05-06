#ifndef __SYS_H
#define __SYS_H
#include "stm32f10x.h"
#include "Rcc.hpp"
#include "Nvic.hpp"
#include "Gpio.hpp"
#include "map.hpp"
#include "delay.hpp"
#include "riselib/riselib.h"

#define LEN_MAX 512 //最大字节数
#define LEN_MID 128 //中等字节数

extern s32 reTry;

//位操作
#define MEM(addr)      (*addr)
//#define setMem(addr,where,content)  (addr &= ~(where);addr |= (content);)
//#define setBit(addr,where)          (addr |= (1<<where);)
//#define clearBit(addr,where)        (addr &= ~(1<<where);)
#define BITBAND(addr,bit)        ((vu32*)(0x42000000+((addr&0xFFFFF)<<5)+(bit<<2)))//位带操作
void setMem(volatile void* p, u32 where, u32 content);
void setBit(volatile void* p, u8 where);
void clearBit(volatile void* p, u8 where);
ErrorStatus run(u8 (*func)(), u32 times);
//以下为汇编函数
void MSR_MSP(u32 addr);//设置堆栈地址

#endif
