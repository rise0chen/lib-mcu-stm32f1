#ifndef __BUFRCV_H
#define __BUFRCV_H
#include "sys.h"

class BufRcv {
 public:
  uint16_t len;       //接收长度
  char buf[LEN_MAX];  //接收的字符串

  BufRcv() {}
  BufRcv(uint8_t s, uint8_t e, void (*fun)(char* msg, uint16_t len));
  void setFlag(uint8_t s, uint8_t e);
  void setFunFinish(void (*fun)(char* msg, uint16_t len));
  void rcv(uint8_t res);

 private:
  uint8_t flagStart;  //字符串起始标志
  uint8_t flagEnd;    //字符串终止标志
  uint8_t status;
  void (*funFinish)(char* msg, uint16_t len);
};

#endif  //__BUFRCV_H
