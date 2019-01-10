/*************************************************
Copyright (C), 2018-2028, Crise Tech. Co., Ltd.
File name: format.c
Author: rise0chen
Version: 1.0
Date: 2018.4.26
Description: 帧传输数据格式
Usage:
History:
        rise0chen   2018.4.26   编写注释
*************************************************/
#include "format.h"

FormatStruct* format_new() {
  FormatStruct* self = (FormatStruct*)malloc(sizeof(FormatStruct));
  self->bufRecver = bufRecver_new(FRAME_LEN_MAX, bufRecver_flagNewBuf);
  self->buf = self->bufRecver->buf;
  self->bufLen = &self->bufRecver->bufLen;
  self->msg = (Format_FieldStruct*)self->buf;
  return self;
}
void format_setVar(FormatStruct* self,
                   Format_FieldEnum field,
                   uint8_t* data,
                   uint16_t dataLen) {
  uint8_t* buf = self->buf;
  uint16_t* bufLen = self->bufLen;

  if (field == format_field_body) {
    *bufLen = format_field_body + dataLen;
  }
  for (uint16_t i = 0; i < dataLen; i++) {
    buf[field + i] = data[i];
  }
}
uint8_t* format_getData(FormatStruct* self) {
  uint8_t* buf = self->buf;
  uint16_t* bufLen = self->bufLen;

  memset(&buf[format_field_leading], 0x55, 3);
  buf[format_field_startTag + 0] = 0xD5;
  buf[format_field_startTag + 1] = 0x7E;

  uint16_t crc = getCrc16(&buf[format_field_startTag], *bufLen - 3);
  buf[(*bufLen)++] = (uint8_t)(crc >> 8);
  buf[(*bufLen)++] = (uint8_t)(crc);
  buf[(*bufLen)++] = 0x7D;
  buf[(*bufLen)++] = 0x0A;
  buf[(*bufLen)++] = 0x00;
  buf[(*bufLen)++] = 0x00;
  buf[(*bufLen)++] = 0x00;

  return buf;
}
Format_FieldStruct* format_getMsg(FormatStruct* self) {
  uint8_t* buf = self->buf;
  uint16_t* bufLen = self->bufLen;
  Format_FieldStruct* msg = self->msg;

  if (*bufLen < 30) {
    return NULL;
  }
  uint16_t crc = getCrc16(&buf[format_field_startTag], *bufLen - 10);
  if ((buf[*bufLen - 7] << 8 | buf[*bufLen - 6]) == crc) {
    memcpy(&buf[format_field_crc], &buf[*bufLen - 7], 7);  //将CRC等复制到尾部

    return msg;
  } else {
    return NULL;
  }
}
