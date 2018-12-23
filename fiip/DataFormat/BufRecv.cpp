/*************************************************
Copyright (C), 2018-2028, Crise Tech. Co., Ltd.
File name: BufRecv.cpp
Author: rise0chen
Version: 1.0
Date: 2018.4.26
Description: 用于单字节接收字符串
Usage:
        #include "BufRecv.hpp"
        BufRecv receiver(0x00, 0x0D, fun_recvfinish);
        char data = 0x30;
        receiver.recv(&data, 1);
History:
        rise0chen   2018.4.26   初步完成
*************************************************/
#include "BufRecv.hpp"

/*************************************************
Function: BufRecv::BufRecv
Description: BufRecv类的构造函数
Calls: setFlag, setFunFinish
Called By: usart.cpp
Input:
        s    字符串的起始标识
        e    字符串的终止标识
        fun  字符串接收完成后的执行函数
Return: BufRecv类
*************************************************/
BufRecv::BufRecv(uint16_t bufLen) {
  buf = new uint8_t(bufLen);
  bufLenMax = bufLen;
  setFlag(0x00, 0x0D);
  setFunFinish(nothing);
}

/*************************************************
Function: BufRecv::setFlag
Description: 设置起始/终止标志
Calls:
Called By: BufRecv
Input:
        s    字符串的起始标识
        e    字符串的终止标识
Return: void
*************************************************/
void BufRecv::setFlag(uint8_t s, uint8_t e) {
  flagStart = s;
  flagEnd = e;
  status = 0;
}

/*************************************************
Function: BufRecv::setFunFinish
Description: 设置接收完成后的执行函数
Calls:
Called By: BufRecv
Input:
        fun  字符串接收完成后的执行函数
Return: void
*************************************************/
void BufRecv::setFunFinish(void (*fun)(uint8_t* buf, uint16_t bufLen)) {
  funFinish = fun;
}

/*************************************************
Function: BufRecv::recv
Description: 接收字节串
            | bit7 | bit6 | bit5 | bit4 | bit3 | bit2 | bit1 | bit0 |
            |    开始     |    结束     |            预留           |
Calls:
Called By: usart.cpp
Input:
        data     需要接收的字节串
        dataLen  字节串长度
Return: uint8_t 0接收完成 1未完成
*************************************************/
uint8_t BufRecv::recv(uint8_t* data, uint8_t dataLen) {
  uint8_t i;
  uint8_t res;
  uint8_t* bufData = buf;

  for (i = 0; i < dataLen; i++) {
    res = data[i];
    if ((status >> 6) != 3) {   //未开始
      if (flagStart == 0x00) {  //无开始标志
        if (res != 0x00) {
          status |= 0xC0;
          bufLen = 0;
          bufData[(bufLen)++] = res;
        }
      } else if (flagStart == res) {  //开始
        status |= 0xC0;
        bufLen = 0;
        bufData[(bufLen)++] = res;
      }
    } else if ((status >> 4) != 0xf) {  //未结束
      bufData[(bufLen)++] = res;
      if (bufLen >= bufLenMax - 3) {
        status = 0;
        bufLen = 0;  //数据过多
      }
      if (res == flagEnd) {  //结束
        status |= 0x30;
        bufData[bufLen] = 0x00;

        funFinish(bufData, bufLen);  //执行函数
        status = 0;
        return 0;
      }
    } else {
      status = 0;
      bufLen = 0;
    }
  }
  return 1;
}
