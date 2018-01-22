#ifndef __SYS_H
#define __SYS_H
#include "stm32f10x.h"
#include "rcc.hpp"
#include "std.hpp"
#include "delay.hpp"
#include "nvic.hpp"
#include "gpio.hpp"

#include "usart.hpp"
#include "flash.hpp"
#include "map.hpp"
#include "pwr.hpp"
#include "wdg.hpp"
#include "tim.hpp"

#define _C extern "C"//汇编语言中调用的函数,需要使用

typedef enum {//状态码
	SUCCESS  = 0x00,//成功
	OVERTIME = 0x01,//超时
	ERROR    = 0xFF,//未知错误
} ErrorStatus;

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

//位操作
#define MEM(addr)      (*addr)
//#define SetMem(addr,where,content)  (addr &= ~(where);addr |= (content);)
//#define SetBit(addr,where)          (addr |= (1<<where);)
//#define ClearBit(addr,where)        (addr &= ~(1<<where);)
#define BITBAND(addr,bit)        ((vu32*)(0x42000000+((addr&0xFFFFF)<<5)+(bit<<2)))//位带操作


extern s32 reTry;

void SetMem(volatile void* p, u32 where, u32 content);
void SetBit(volatile void* p, u8 where);
void ClearBit(volatile void* p, u8 where);
ErrorStatus run(u8 (*func)(), u32 times);
//以下为汇编函数
void MSR_MSP(u32 addr);//设置堆栈地址

#endif
