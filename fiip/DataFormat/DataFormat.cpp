/*************************************************
Copyright (C), 2018-2028, Crise Tech. Co., Ltd.
File name: protocol.cpp
Author: rise0chen
Version: 1.0
Date: 2018.4.26
Description: 帧传输通信协议
Usage:
History:
        rise0chen   2018.4.26   编写注释
*************************************************/
#include "DataFormat.hpp"

ESCStruct DataFormat::esctype[ESC_LEN] = {
    {{1, 2}, {{0x7E}, {0x7B, 0x01}}},
    {{1, 2}, {{0x7D}, {0x7B, 0x02}}},
    {{1, 2}, {{0x7B}, {0x7B, 0x03}}},
};

DataFormat::DataFormat() : BufRecv(BUF_LEN_MAX) {
  setFlag(0x7E, 0x7D);
  msg = (FieldStruct*)buf;
  index[0] = 0x00;
  index[1] = 0x01;
}
void DataFormat::setVar(FieldEnum field, uint8_t* data, uint16_t dataLen) {
  if (field == field_body) {
    buf[field_len] = (uint8_t)(dataLen >> 8);
    buf[field_len + 1] = (uint8_t)(dataLen);
    bufLen = 30 + dataLen;
  }
  for (uint16_t i = 0; i < dataLen; i++) {
    buf[field + i] = data[i];
  }
}
uint8_t* DataFormat::getData() {
  buf[field_startTag] = 0x7E;
  if (index[1] == 0xff) {
    index[0]++;
    index[1] = 0x00;
  } else {
    index[1]++;
  }
  buf[field_index + 0] = index[0];
  buf[field_index + 1] = index[1];
  uint16_t crc = getCrc16(buf, bufLen);
  buf[bufLen++] = (uint8_t)(crc >> 8);
  buf[bufLen++] = (uint8_t)(crc);
  bufLen = escape((char*)&buf[1], bufLen - 1, esctype, ESC_LEN, 1) + 1;
  buf[bufLen++] = 0x7d;

  return buf;
}
DataFormat::FieldStruct* DataFormat::getMsg() {
  bufLen = escape((char*)&buf[0], bufLen, esctype, ESC_LEN, 0);
  uint16_t crc = getCrc16(buf, bufLen - 3);
  if ((buf[bufLen - 3] << 8 | buf[bufLen - 2]) == crc) {
    return msg;
  } else {
    return NULL;
  }
}
