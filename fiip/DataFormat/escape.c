/*************************************************
Copyright (C), 2018-2028, Crise Tech. Co., Ltd.
File name: escape.c
Author: rise0chen
Version: 1.0
Date: 2018.4.26
Description: 字符转义/还原
Usage:
        #include "escape.h"
        #define ESC_LEN  3
        ESCStruct esctype[ESC_LEN]={
                {{1,2},{{0x7E},{0x7B,0x01}}},
                {{1,2},{{0x7D},{0x7B,0x02}}},
                {{1,2},{{0x7B},{0x7B,0x03}}},
        };
        escape(usart1.tx.buf, usart1.tx.len, esctype, ESC_LEN, 1); //转义
        escape(usart1.rx.buf, usart1.rx.len, esctype, ESC_LEN, 0); //还原
History:
        rise0chen   2018.4.26   编写注释
*************************************************/
#include "escape.h"

/*************************************************
Function: escape
Description: 字符转义/还原
Input:
        bufin   待转义的数据
        lenin   待转义的数据长度
        type    转义规则
        typelen 转义规则的长度
        way     方式 0还原  1转义
Return: 转义/还原后的长度
*************************************************/
unsigned short escape(char* bufin,
                      unsigned short lenin,
                      ESCStruct* type,
                      unsigned char typelen,
                      unsigned char way) {
  unsigned short i, j, k, outi = 0;
  char bufout[512];

  for (i = 0; i < lenin; i++) {                     // buf循环i
    for (j = 0; j < typelen; j++) {                 //转义结构体循环j
      for (k = 0; k < type[j].len[1 ^ way]; k++) {  //转义标识符循环j
        if (bufin[i + k] != type[j].buf[1 ^ way][k]) {
          break;
        }
      }
      if (k == type[j].len[1 ^ way]) {
        for (k = 0; k < type[j].len[0 ^ way]; k++) {  //转义标识符循环j
          bufout[outi++] = type[j].buf[0 ^ way][k];
        }
        i = i + type[j].len[1 ^ way] - 1;
        break;
      }
    }
    if (j == typelen) {
      bufout[outi++] = bufin[i];
    }
  }
  for (i = 0; i < outi; i++) {
    bufin[i] = bufout[i];
  }
  return outi;
}
