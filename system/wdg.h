#ifndef __WDG_H
#define __WDG_H
#include "sys.h"

void iwdg_config(uint8_t prer,uint16_t rlr);
void iwdg_feed(void);
void wwdg_config(uint8_t tr,uint8_t wr,uint8_t fprer);

#endif //__WDG_H
