#ifndef __TOUCH_H
#define __TOUCH_H
#include "sys.hpp"


extern u16 Touch_X, Touch_Y;//坐标值


#define Touch_In		GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_8)

void Touch_init(void);//触摸屏初始化
u8 Touch_GetPoint(void);//获取坐标
u8 Touch_Calibrate(void);//校准
void Touch_DrawCross(u16 x, u16 y);//十字叉线

void EXTI15_10_IRQHandler(void);

#endif //__TOUCH_H
