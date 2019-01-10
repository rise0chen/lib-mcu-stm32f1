#ifndef __FIIP_H
#define __FIIP_H
#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include "./frame/format.h"
#include "./link/link.h"
#include "./link/route.h"

typedef struct {
  uint16_t type;
  void (*solve)(Format_FieldStruct*, LinkCfgStruct*);
} ProtocolStruct;  //通信协议结构

extern uint8_t fiip_centerId[8];
extern uint8_t fiip_broadId[8];
extern FormatStruct* fiip_rxd;

void fiip_init();
void fiip_setId(uint8_t* id, uint8_t* key);
void fiip_addServer(uint8_t type, LinkServerStruct* linkServer);
void fiip_addProtocol(uint16_t type, ProtocolStruct* protocol);
void fiip_sendOriginal(uint8_t* data, uint16_t dataLen, LinkCfgStruct* link);
void fiip_send(FormatStruct* frame, uint8_t* id, LinkCfgStruct* link);
void fiip_broadcast(FormatStruct* frame);
void fiip_recv(uint8_t* buf, uint16_t bufLen, LinkCfgStruct* link);
void fiip_connect(uint8_t* id, LinkCfgStruct* link);

#ifdef __cplusplus
}
#endif
#endif  //__FIIP_H
