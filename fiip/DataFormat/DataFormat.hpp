#ifndef __DATAFORMAT_H
#define __DATAFORMAT_H
#include <stdint.h>
#include <string.h>
#include "BufRecv.hpp"
#include "crc.h"
#include "escape.h"

#define BUF_LEN_MAX 256
#define ESC_LEN 3

class DataFormat : public BufRecv {
 public:
  typedef struct {
    uint8_t startTag[1];         //起始标识
    uint8_t srcAddr[8];          //源地址
    uint8_t dstAddr[8];          //目标地址
    uint8_t reserve[2];          //预留
    uint8_t cryptTag[1];         //加密算法标识
    uint8_t cryptKey[4];         //加密算法密钥
    uint8_t cmd[2];              //命令标识
    uint8_t index[2];            //命令序列号
    uint8_t len[2];              //消息体长度
    uint8_t body[BUF_LEN_MAX - 30];  //消息体
  } FieldStruct;                 //数据结构
  typedef enum {
    field_startTag = 0,
    field_srcAddr = 1,
    field_dstAddr = 9,
    field_reserve = 17,
    field_cryptTag = 19,
    field_cryptKey = 20,
    field_cmd = 24,
    field_index = 26,
    field_len = 28,
    field_body = 30
  } FieldEnum;
  FieldStruct* msg;
  static ESCStruct esctype[ESC_LEN];

  DataFormat();
  void setVar(FieldEnum, uint8_t*, uint16_t);
  uint8_t* getData();
  FieldStruct* getMsg();

 private:
  uint8_t index[2];  //序列号
};
#endif  //__DATAFORMAT_H
