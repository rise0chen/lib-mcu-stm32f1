#ifndef __PWR_H
#define __PWR_H
#include "sys.h"

namespace pwr {
void WFI_SET(void);
void reset(void);              //系统软复位
void sleep(uint8_t next = 0);  //睡眠模式
void stop(void);               //停止模式
void standby(void);            //待机模式
}  // namespace pwr

#endif  //__PWR_H
