#ifndef __ULTRASONIC_H
#define __ULTRASONIC_H
#include "sys.h"

class Ultrasonic {
 public:
  uint16_t distance;  //单位毫米mm

  Ultrasonic(Gpio* TRIG, Gpio* ECHO);
  uint16_t get(uint8_t num = 1);

 private:
  Gpio* TRIG;
  Gpio* ECHO;
};
#endif  //__TEMPLATE_H
