#include "./linkUsart.h"
static void linkUsart_send(uint8_t* data,
                           uint16_t dataLen,
                           LinkCfgStruct* rlink);
static void linkUsart_recv(uint8_t IRQn, uint32_t status);
static LinkCfgStruct* servers[3];

LinkCfgStruct* linkUsart_start(uint8_t* dev, uint32_t speed) {
  if (dev[0] == '1') {
    if (servers[0])
      return servers[0];
  } else if (dev[0] == '2') {
    if (servers[1])
      return servers[1];
  } else if (dev[0] == '3') {
    if (servers[2])
      return servers[2];
  }

  LinkCfgStruct* link = link_new(NULL);

  link->type = linkType_usart;
  link->status = linkStatus_server;
  mem_cpy(link->address, dev, 1);
  link->port = speed;
  if (link->address[0] == '1') {
    link->fd = &usart1;
    servers[0] = link;
  } else if (link->address[0] == '2') {
    link->fd = &usart2;
    servers[1] = link;
  } else if (link->address[0] == '3') {
    link->fd = &usart3;
    servers[2] = link;
  }
  link->send = linkUsart_send;
  link->recv = fiip_recv;

  Usart* fd = (Usart*)link->fd;
  it_addListener(fd->IRQn, (void*)linkUsart_recv);
  return link;
}

void linkUsart_send(uint8_t* data, uint16_t dataLen, LinkCfgStruct* rlink) {
  Usart* fd = (Usart*)rlink->fd;

  fd->send((char*)data, dataLen);
}

void linkUsart_recv(uint8_t IRQn, uint32_t status) {
  LinkCfgStruct* self = NULL;
  Usart* fd;
  if (IRQn == usart1.IRQn) {
    self = servers[0];
  } else if (IRQn == usart2.IRQn) {
    self = servers[1];
  } else if (IRQn == usart3.IRQn) {
    self = servers[2];
  }else{
    return;
  }
  fd = (Usart*)self->fd;

  self->recv(fd->rxBuf, fd->rxNum, self);
}
