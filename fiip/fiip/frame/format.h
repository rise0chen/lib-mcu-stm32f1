#ifndef __FORMAT_H
#define __FORMAT_H
#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include "bufRecver.h"
#include "crc.h"

#define FRAME_LEN_MAX 256

typedef struct {
  uint8_t leading[3];                //前导
  uint8_t startTag[2];               //起始标识
  uint8_t srcAddr[8];                //源地址
  uint8_t dstAddr[8];                //目标地址
  uint8_t type[2];                   //协议类型
  uint8_t body[FRAME_LEN_MAX - 30];  //消息体
  uint8_t crc[2];                    // CRC校验
  uint8_t endTag[2];                 //结束标识
  uint8_t trailing[3];               //后继
} Format_FieldStruct;                //数据结构
typedef enum {
  format_field_leading = 0,
  format_field_startTag = 3,
  format_field_srcAddr = 5,
  format_field_dstAddr = 13,
  format_field_type = 21,
  format_field_body = 23,
  format_field_crc = FRAME_LEN_MAX - 7,
  format_field_endTag = FRAME_LEN_MAX - 5,
  format_field_trailing = FRAME_LEN_MAX - 3,
} Format_FieldEnum;

typedef struct {
  BufRecverStruct* bufRecver;
  uint8_t* buf;
  uint16_t* bufLen;
  Format_FieldStruct* msg;
} FormatStruct;       //数据结构

FormatStruct* format_new(void);
void format_setVar(FormatStruct* self,
                   Format_FieldEnum field,
                   uint8_t* data,
                   uint16_t dataLen);
uint8_t* format_getData(FormatStruct* self);
Format_FieldStruct* format_getMsg(FormatStruct* self);

#ifdef __cplusplus
}
#endif
#endif  //__FORMAT_H
