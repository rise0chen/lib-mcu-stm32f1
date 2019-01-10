#include "bufRecver.h"
static void doNothing(uint8_t* buf, uint16_t bufLen) {}

uint8_t* bufRecver_flagNewBuf = (uint8_t*)"bufRecver_flagNewBuf";

/*************************************************
Function: bufRecver_new
Description: BufRecver类的构造函数
Input:
        lenMax Buffer最大长度
        buf    内存指针或flagNewBuf
Return: BufRecverStruct
*************************************************/
BufRecverStruct* bufRecver_new(uint16_t lenMax, uint8_t* buf) {
  BufRecverStruct* self = (BufRecverStruct*)malloc(sizeof(BufRecverStruct));

  if (buf == bufRecver_flagNewBuf) {
    self->buf = (uint8_t*)malloc(lenMax);
  } else {
    self->buf = buf;
  }
  self->bufLen = 0;
  self->_bufLenMax = lenMax;
  self->_leading[0] = 0x55;
  self->_flagStart[0] = 0xD5;
  self->_flagStart[1] = 0x7E;
  self->_flagEnd[0] = 0x7D;
  self->_flagEnd[1] = 0x0A;
  self->_trailing[0] = 0x00;
  self->_status = 0;
  self->_funFinish = doNothing;
  return self;
}

/*************************************************
Function: bufRecver_setFunFinish
Description: 设置接收完成后的执行函数
Input:
        fun  字符串接收完成后的执行函数
Return: void
*************************************************/
void bufRecver_setFunFinish(BufRecverStruct* self,
                            void (*fun)(uint8_t* buf, uint16_t bufLen)) {
  self->_funFinish = fun;
}

/*************************************************
Function: bufRecver_recv
Description: 接收字节串
            | bit7 bit6  | bit5    | bit4    | bit3    | bit2    | bit1 bit0  |
            | 3 次后继符 | 结束 0A | 结束 7D | 起始 7E | 起始 D5 | 3 次前导符 |
Input:
        data     需要接收的字节串
        dataLen  字节串长度
Return: int16_t 0接收完成 1未完成
*************************************************/
int16_t bufRecver_recv(BufRecverStruct* self, uint8_t* data, uint16_t dataLen) {
  int16_t ret = -1;
  uint8_t res;
  uint8_t* status = &self->_status;
  uint8_t* leading = self->_leading;
  uint8_t* flagStart = self->_flagStart;
  uint8_t* flagEnd = self->_flagEnd;
  uint8_t* trailing = self->_trailing;
  uint8_t* bufData = self->buf;
  uint16_t* bufLen = &self->bufLen;
  uint16_t bufLenMax = self->_bufLenMax;

  for (uint16_t i = 0; i < dataLen; i++) {
    res = data[i];

    if (*status < 0x03) {  //小于3次前导
      if (res == leading[0]) {
        *bufLen = *status;
        bufData[(*bufLen)++] = res;
        *status += (1 << 0);
      } else {
        *status = 0;
      }
    } else if (*status < 0x07) {  //起始0
      if (res == leading[0]) {
      } else if (res == flagStart[0]) {
        bufData[(*bufLen)++] = res;
        *status += (1 << 2);
      } else {
        *status = 0;
      }
    } else if (*status < 0x0F) {  //起始1
      if (res == flagStart[1]) {
        bufData[(*bufLen)++] = res;
        *status += (1 << 3);
      } else {
        *status = 0;
      }
    } else if (*status < 0x1F) {  //结束符0
      if (res == flagEnd[0]) {
        bufData[(*bufLen)++] = res;
        *status += (1 << 4);
      } else {
        if (*bufLen >= bufLenMax - 10) {
          *status = 0;  //超出最大长度
          ret = 9;
        } else {
          bufData[(*bufLen)++] = res;  //正在接收数据
          ret = 1;
        }
      }
    } else if (*status < 0x3F) {  //结束符1
      if (res == flagEnd[1]) {
        bufData[(*bufLen)++] = res;
        *status += (1 << 5);
      } else {
        *status = 0;
      }
    } else if (*status < 0xFF) {  //小于3次后继
      if (res == trailing[0]) {
        bufData[(*bufLen)++] = res;
        *status += (1 << 6);

        if (*status == 0xFF) {                 //接收完毕
          self->_funFinish(bufData, *bufLen);  //执行函数
          *status = 0;
          return 0;
        }
      } else {
        *status = 0;
      }
    } else {
      *status = 0;
    }
  }
  return ret;
}
