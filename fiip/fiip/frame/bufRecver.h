#ifndef __BUFRECVER_H
#define __BUFRECVER_H
#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdlib.h>

typedef struct {
  uint8_t* buf;
  uint16_t bufLen;
  uint16_t _bufLenMax;
  uint8_t _leading[1];    //前导
  uint8_t _flagStart[2];  //字符串起始标志
  uint8_t _flagEnd[2];    //字符串终止标志
  uint8_t _trailing[1];   //后继
  uint8_t _status;
  void (*_funFinish)(uint8_t* buf, uint16_t bufLen);
} BufRecverStruct;  // buf接收器结构

extern uint8_t* bufRecver_flagNewBuf;

BufRecverStruct* bufRecver_new(uint16_t lenMax, uint8_t* buf);
void bufRecver_setFunFinish(BufRecverStruct* self,
                            void (*fun)(uint8_t* buf, uint16_t bufLen));
int16_t bufRecver_recv(BufRecverStruct* self, uint8_t* data, uint16_t len);

#ifdef __cplusplus
}
#endif
#endif  //__BUFRECVER_H
