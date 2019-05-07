#ifndef __DS18B20_H
#define __DS18B20_H
#include "sys.h"

//温度传感器
class DS18B20 {
 public:
  DS18B20(Gpio* P_DQ);
  void setGPIO(Gpio* P_DQ);
  uint8_t getTemp(void);

 private:
  Gpio* DQ;

  void init(void);
  void write(uint8_t dat);
  uint8_t read(void);
};

#endif  //__DS18B20_H
