#ifndef __STDP_FORMAT_H
#define __STDP_FORMAT_H
#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include "../fiip/frame/format.h"

typedef struct {
  uint8_t leading[3];                //前导
  uint8_t startTag[2];               //起始标识
  uint8_t srcAddr[8];                //源地址
  uint8_t dstAddr[8];                //目标地址
  uint8_t type[2];                   //协议类型
  uint8_t cryptTag[1];               //加密算法标识
  uint8_t cryptKey[4];               //加密算法密钥
  uint8_t cmd[2];                    //命令标识
  uint8_t index[2];                  //命令序列号
  uint8_t len[2];                    //消息体长度
  uint8_t body[FRAME_LEN_MAX - 41];  //消息体
  uint8_t crc[2];                    // CRC校验
  uint8_t endTag[2];                 //结束标识
  uint8_t trailing[3];               //后继
} Stdp_FieldStruct;                  //数据结构
typedef enum {
  stdp_field_leading = 0,
  stdp_field_startTag = 3,
  stdp_field_srcAddr = 5,
  stdp_field_dstAddr = 13,
  stdp_field_type = 21,
  stdp_field_cryptTag = 23 + 0,
  stdp_field_cryptKey = 23 + 1,
  stdp_field_cmd = 23 + 5,
  stdp_field_index = 23 + 7,
  stdp_field_len = 23 + 9,
  stdp_field_body = 23 + 11,
  stdp_field_crc = FRAME_LEN_MAX - 7,
  stdp_field_endTag = FRAME_LEN_MAX - 5,
  stdp_field_trailing = FRAME_LEN_MAX - 3,
} Stdp_FieldEnum;

typedef struct {
  FormatStruct* frame;
  uint8_t* buf;
  uint16_t* bufLen;
  Stdp_FieldStruct* msg;
  uint8_t _index[2];  //序列号
} StdpStruct;         //数据结构

StdpStruct* stdp_new(void);
void stdp_setVar(StdpStruct* self,
                 Stdp_FieldEnum field,
                 uint8_t* data,
                 uint16_t dataLen);
uint8_t* stdp_getData(StdpStruct* self);
Stdp_FieldStruct* stdp_getMsg(StdpStruct* self);

#ifdef __cplusplus
}
#endif
#endif  //__STDP_FORMAT_H
