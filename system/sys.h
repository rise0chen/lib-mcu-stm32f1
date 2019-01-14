#ifndef __SYS_H
#define __SYS_H
#include "stm32f10x.h"
#include "nvic.h"
#include "rcc.h"
#include "std.h"
#include "delay.h"
// #define _C extern "C"//汇编语言中调用的函数,需要使用

#define LEN_MAX 512 //最大字节数
#define LEN_MID 128 //中等字节数

typedef enum {//状态码
	SUCCESS  = 0x00,//成功
	OVERTIME = 0x01,//超时
	ERROR    = 0xFF,//未知错误
} ErrorStatus;


extern int32_t reTry;

//位操作
#define MEM(addr)      (*addr)
//#define setMem(addr,where,content)  (addr &= ~(where);addr |= (content);)
//#define setBit(addr,where)          (addr |= (1<<where);)
//#define clearBit(addr,where)        (addr &= ~(1<<where);)
#define BITBAND(addr,bit)        ((volatile uint32_t*)(0x42000000+((addr&0xFFFFF)<<5)+(bit<<2)))//位带操作
void setMem(volatile void* p, uint32_t where, uint32_t content);
void setBit(volatile void* p, uint8_t where);
void clearBit(volatile void* p, uint8_t where);
ErrorStatus run(uint8_t (*func)(), uint32_t times);

#endif
