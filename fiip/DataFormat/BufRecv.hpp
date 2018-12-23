#ifndef __BUFRECV_H
#define __BUFRECV_H
#include <stdint.h>

class BufRecv {
 public:
  uint8_t* buf;
  uint16_t bufLen;

  BufRecv(uint16_t bufLen);
  void setFlag(uint8_t s, uint8_t e);
  void setFunFinish(void (*fun)(uint8_t* buf, uint16_t bufLen));
  uint8_t recv(uint8_t* data, uint8_t len = 1);

 private:
  uint16_t bufLenMax;
  uint8_t flagStart;  //字符串起始标志
  uint8_t flagEnd;    //字符串终止标志
  uint8_t status;
  void (*funFinish)(uint8_t* buf, uint16_t bufLen);
  static void nothing(uint8_t* buf, uint16_t bufLen){};
};

#endif  //__BUFRECV_H
