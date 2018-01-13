#ifndef __WDG_H
#define __WDG_H
#include "sys.hpp"

void IWDG_Config(u8 prer,u16 rlr);
void IWDG_Feed(void);

void WWDG_Config(u8 tr,u8 wr,u8 fprer);

#endif //__WDG_H
