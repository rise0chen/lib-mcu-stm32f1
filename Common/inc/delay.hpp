#ifndef __DELAY_H
#define __DELAY_H
#include "sys.hpp"

#define  DWT_CR      *(volatile u32 *)0xE0001000
#define  DWT_CYCCNT  *(volatile u32 *)0xE0001004
#define  DEM_CR      *(volatile u32 *)0xE000EDFC

void  delay(u16 time);//nms
void  delay_us(u32 nus);
#define delay_ms(nms)         delay_us(nms*1000)

void  tic(void);
u32  toc(u8 way=0);

#endif //__DELAY_H
