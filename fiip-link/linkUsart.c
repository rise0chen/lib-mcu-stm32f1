#include "./linkUsart.h"
static LinkServerStruct* linkServer;
void linkUsart_start(LinkServerStruct* self);
void linkUsart_send(LinkServerStruct* self,
                    uint8_t* data,
                    uint16_t dataLen,
                    LinkCfgStruct* link);

void startUsart(uint8_t* dev, uint32_t speed) {
  LinkCfgStruct* link = (LinkCfgStruct*)malloc(sizeof(LinkCfgStruct));
  linkServer = (LinkServerStruct*)malloc(sizeof(LinkServerStruct));
  link->type = linkType_usart;
  link->address = dev;
  link->port = speed;

  linkServer->link = link;
  linkServer->start = linkUsart_start;
  linkServer->send = linkUsart_send;
  linkServer->recv = fiip_recv;

  fiip_addServer(linkType_usart, linkServer);
  linkServer->start(linkServer);
}

void linkUsart_start(LinkServerStruct* self) {}

void linkUsart_send(LinkServerStruct* self,
                    uint8_t* data,
                    uint16_t dataLen,
                    LinkCfgStruct* dstLink) {
  if (dstLink->address[0] == '1') {
    usart1.send((char*)data, dataLen);
  } else if (dstLink->address[0] == '2') {
    usart2.send((char*)data, dataLen);
  } else if (dstLink->address[0] == '3') {
    usart3.send((char*)data, dataLen);
  }
}
/*
_C void USART2_IRQHandler(void) {
  uint8_t recvBuf[1];
  int recvNum = 1;

  if (USART2->SR & (1 << 5)) {
    recvBuf[0] = USART2->DR;
    // to do something
    LinkCfgStruct* srcLink = (LinkCfgStruct*)malloc(sizeof(LinkCfgStruct));
    srcLink->type = linkType_usart;
    srcLink->address = (uint8_t*)malloc(8);
    srcLink->address[0] = linkServer->link->address[0];
    srcLink->port = linkServer->link->port;
    linkServer->recv(recvBuf, recvNum, srcLink);
  }
};
*/
