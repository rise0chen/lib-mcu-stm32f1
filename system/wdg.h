#ifndef __WDG_H
#define __WDG_H
#include "sys.h"

void iwdg_config(u8 prer,u16 rlr);
void iwdg_feed(void);
void wwdg_config(u8 tr,u8 wr,u8 fprer);

#endif //__WDG_H
