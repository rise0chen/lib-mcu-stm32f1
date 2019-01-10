#include "./fiip.h"

uint8_t fiip_centerId[8] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
uint8_t fiip_broadId[8] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
FormatStruct* fiip_rxd;

static LinkServerStruct* linkServers[8];
static uint16_t linkServersLen;
static ProtocolStruct* protocols[16];
static uint16_t protocolsLen;
static uint8_t* myId;
static uint8_t* myKey;

void fiip_init() {
  fiip_rxd = format_new();
}
void fiip_setId(uint8_t* id, uint8_t* key) {
  myId = id;
  myKey = key;
}
void fiip_addServer(uint8_t type, LinkServerStruct* linkServer) {
  if (linkServersLen < 8) {
    linkServers[linkServersLen] = linkServer;
    linkServers[linkServersLen]->type = type;
    fiip_connect(myId, linkServer->link);
    linkServersLen++;
  }
}
void fiip_addProtocol(uint16_t type, ProtocolStruct* protocol) {
  if (protocolsLen < 16) {
    protocols[protocolsLen] = protocol;
    protocols[protocolsLen]->type = type;
    protocolsLen++;
  }
}
void fiip_sendOriginal(uint8_t* data, uint16_t dataLen, LinkCfgStruct* link) {
  printf("link type:%02X; %d.%d.%d.%d:%d.\n", link->type, link->address[0],
         link->address[1], link->address[2], link->address[3], link->port);
  printf("send:");
  for (uint8_t i = 0; i < dataLen; i++) {
    printf("%02X", data[i]);
  }
  printf("\n");
  for (uint8_t i = 0; i < linkServersLen; i++) {
    if (linkServers[i]->type == link->type) {
      linkServers[i]->send(linkServers[i], data, dataLen, link);
    }
  }
}
void fiip_send(FormatStruct* frame, uint8_t* id, LinkCfgStruct* link) {
  if (link == NULL) {
    if (id == NULL) {
      id = frame->msg->dstAddr;
    } else {
      format_setVar(frame, format_field_dstAddr, id, 8);
    }
    link = route_find(id);
  }
  if (link) {
    format_setVar(frame, format_field_srcAddr, myId, 8);
    format_getData(frame);
    fiip_sendOriginal(frame->buf, *frame->bufLen, link);
  } else {
    printf("no route,can't send.\n\n");
  }
}
void fiip_broadcast(FormatStruct* frame) {
  format_setVar(frame, format_field_srcAddr, myId, 8);
  format_getData(frame);
  for (uint16_t i = 0; i < route_linksLen; i++) {
    fiip_sendOriginal(frame->buf, *frame->bufLen, route_links[i]);
  }
}
void fiip_recv(uint8_t* buf, uint16_t bufLen, LinkCfgStruct* link) {
  printf("recv:");
  for (uint8_t i = 0; i < bufLen; i++) {
    printf("%02X", buf[i]);
  }
  printf("\n");
  if (bufRecver_recv(fiip_rxd->bufRecver, buf, bufLen) == 0) {
    if (format_getMsg(fiip_rxd)) {
      route_update(fiip_rxd->msg->srcAddr, link);  //更新路由

      if (memcmp(fiip_rxd->msg->dstAddr, myId, 8) == 0) {  //执行
        for (uint8_t i = 0; i < protocolsLen; i++) {
          if (protocols[i]->type ==
              (fiip_rxd->msg->type[0] << 8 | fiip_rxd->msg->type[1])) {
            protocols[i]->solve(fiip_rxd->msg, link);
            return;
          }
        }
        printf("no protocol type.\n");
      } else {  //转发
        LinkCfgStruct* link;
        link = route_find(fiip_rxd->msg->dstAddr);
        if (link) {
          fiip_sendOriginal(fiip_rxd->buf, *fiip_rxd->bufLen, link);
        } else {
          printf("no route,can't forward.\n");
        }
      }
    } else {
      printf("crc error\n");
    }
  } else {
    free(link);
  }
}
void fiip_connect(uint8_t* id, LinkCfgStruct* link) {
  route_remove(id);
  route_update(id, link);
  route_fix(id);
}
