/*************************************************
Copyright (C), 2018-2028, Crise Tech. Co., Ltd.
File name: sys.cpp
Author: rise0chen
Version: 1.0
Date: 2018.4.26
Description: 通用文件,包含常用头文件、函数等
Usage:
        #include "sys.h"
History:
        rise0chen   2018.4.26   编写注释
*************************************************/
#include "sys.h"
#include <string.h>

int32_t reTry = 0x00FFFFFF;

/*************************************************
Function: setMem
Description: 设置寄存器内容
Input:
        p       需要设置的寄存器的地址
        where   在需要设置的bit上置1, 如0b0111代表设置第0~2位
        content 需要设置的内容(<where)如0b0101代表第1、2位置1
Return: void
*************************************************/
void setMem(volatile void* p, uint32_t where, uint32_t content) {
  uint32_t* p0 = (uint32_t*)p;
  *p0 &= ~where;   //清零
  *p0 |= content;  //置数
}

/*************************************************
Function: setBit
Description: 单bit置1
Input:
        p       需要设置的寄存器的地址
        where   需要置1的位置 0~31
Return: void
*************************************************/
void setBit(volatile void* p, uint8_t where) {
  uint32_t* p0 = (uint32_t*)p;
  *p0 |= (1 << where);
}

/*************************************************
Function: clearBit
Description: 单bit置0
Input:
        p       需要设置的寄存器的地址
        where   需要置0的位置 0~31
Return: void
*************************************************/
void clearBit(volatile void* p, uint8_t where) {
  uint32_t* p0 = (uint32_t*)p;
  *p0 &= ~(1 << where);
}

/*************************************************
Function: run
Description: 执行times次，直到(*func)()返回0
Input:
        p       需要设置的寄存器的地址
        where   需要置0的位置 0~31
Return: void
Other: auto a = [](uint8_t b){return b;};//定义局部函数
*************************************************/
ErrorStatus run(uint8_t (*func)(), uint32_t times) {
  reTry = times;
  while ((*func)()) {  //判断条件
    if (reTry-- <= 0) {
      return error_overtime;
    }
  }
  return error_success;
}

/*************************************************
Function: waitBuf
Description: 等待接收到期待的数据
Input:
        where  数据地址
        req    期待的数据
        s      等待时间 0~25秒
Return: 通用错误码
*************************************************/
ErrorStatus waitBuf(char* where, char* req, uint8_t s) {
  uint8_t time = 0;
  if (req[0] != '\0') {
    while (!strstr(where, req)) {
      delay_ms(100);
      if (time++ > 10 * s) {
        return error_overtime;
      }
    }
    return error_success;
  } else {
    return error_other;
  }
}
