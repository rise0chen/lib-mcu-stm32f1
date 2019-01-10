#ifndef __ULTRASONIC_H
#define __ULTRASONIC_H
#include "sys.h"
#include "gpio.h"

typedef struct {
  u16 distance;//单位毫米mm

  GpioStruct *TRIG;
  GpioStruct *ECHO;
} UltrasonicStruct;  //红外线结构

UltrasonicStruct* ultrasonic_new(GpioStruct *TRIG, GpioStruct *ECHO);
u16  ultrasonic_get(UltrasonicStruct* self, u8 num);
	
#endif //__TEMPLATE_H
