#ifndef __ULTRASONIC_H
#define __ULTRASONIC_H
#include "sys.h"
#include "gpio.h"

typedef struct {
  uint16_t distance;//单位毫米mm

  GpioStruct *TRIG;
  GpioStruct *ECHO;
} UltrasonicStruct;  //红外线结构

UltrasonicStruct* ultrasonic_new(GpioStruct *TRIG, GpioStruct *ECHO);
uint16_t  ultrasonic_get(UltrasonicStruct* self, uint8_t num);
	
#endif //__TEMPLATE_H
