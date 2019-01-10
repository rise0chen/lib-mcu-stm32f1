#include "./stdpFormat.h"

StdpStruct* stdp_new() {
  StdpStruct* self = (StdpStruct*)malloc(sizeof(StdpStruct));
  self->frame = format_new();
  self->buf = self->frame->buf;
  self->bufLen = self->frame->bufLen;
  self->msg = (Stdp_FieldStruct*)self->frame->msg;
  self->_index[0] = 0x00;
  self->_index[1] = 0x01;
  return self;
}
void stdp_setVar(StdpStruct* self,
                 Stdp_FieldEnum field,
                 uint8_t* data,
                 uint16_t dataLen) {
  uint8_t* buf = self->frame->buf;
  uint16_t* bufLen = self->frame->bufLen;
  uint8_t* index = self->_index;

  if (field == stdp_field_body) {
    //设置指令序列号index
    if (index[1] == 0xff) {
      index[0]++;
      index[1] = 0x00;
    } else {
      index[1]++;
    }
    buf[stdp_field_index + 0] = index[0];
    buf[stdp_field_index + 1] = index[1];

    //设置消息体长度
    buf[stdp_field_len] = (uint8_t)(dataLen >> 8);
    buf[stdp_field_len + 1] = (uint8_t)(dataLen);
    *bufLen = stdp_field_body + dataLen;
  }
  for (uint16_t i = 0; i < dataLen; i++) {
    buf[field + i] = data[i];
  }
}
uint8_t* stdp_getData(StdpStruct* self) {
  return format_getData(self->frame);
}
Stdp_FieldStruct* stdp_getMsg(StdpStruct* self) {
  return (Stdp_FieldStruct*)format_getMsg(self->frame);
}
