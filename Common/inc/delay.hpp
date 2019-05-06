#ifndef __DELAY_H
#define __DELAY_H
#include "sys.hpp"

#define  DWT_CR      *(volatile uint32_t *)0xE0001000
#define  DWT_CYCCNT  *(volatile uint32_t *)0xE0001004
#define  DEM_CR      *(volatile uint32_t *)0xE000EDFC

void  delay(uint16_t time);//nms
void  delay_us(uint32_t nus);
#define delay_ms(nms)         delay_us(nms*1000)

void  tic(void);
uint32_t  toc(uint8_t way=0);

#endif //__DELAY_H
