#ifndef SYSTICK_H
#define SYSTICK_H
#include "riselib/task.h"
#include "sys.h"

extern uint16_t sysTick_oneTime;
extern uint32_t sysTick_times;

void sysTick_init(uint16_t nms);

#endif  // SYSTICK_H
