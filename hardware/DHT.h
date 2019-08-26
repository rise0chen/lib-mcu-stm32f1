#ifndef DHT_H
#define DHT_H
#include "sys.h"

// DHT温湿度传感器
class DHT {
 public:
  uint8_t temp[2];
  uint8_t humi[2];

  DHT(Gpio* P_DQ);
  void setGPIO(Gpio* P_DQ);
  int8_t getData(void);

 private:
  Gpio* DQ;

  void start(void);
  uint8_t read(void);
};

#endif  // DHT_H
