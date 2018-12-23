#ifndef __PWR_H
#define __PWR_H
#include "sys.h"

void pwr_reset(void);  //系统软复位
void pwr_sleep(u8 next=0);//睡眠模式
void pwr_stop(void);//停止模式
void pwr_standby(void);//待机模式

#endif //__PWR_H
