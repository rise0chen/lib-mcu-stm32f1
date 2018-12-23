#include "Fiip.hpp"
Fiip* fiip = new Fiip();

uint8_t Fiip::centerId[8] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
uint8_t Fiip::broadId[8] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};

void Fiip::setId(uint8_t* id, uint8_t* key) {
  myId = id;
  myKey = key;
  txd->setVar(DataFormat::field_srcAddr, id, 8);
}
void Fiip::addListener(uint16_t port,
                       void (*fun)(DataFormat::FieldStruct* msg)) {
  if (ListenersLen < ListenersMaxLen) {
    Listeners[ListenersLen] = new ListenerStruct;
    Listeners[ListenersLen]->port = port;
    Listeners[ListenersLen]->fun = fun;
    ListenersLen++;
  }
}
void Fiip::solve(DataFormat::FieldStruct* msg, LinkCfgStruct* link) {
  if (memcpy(msg->dstAddr, myId, 8) == 0) {
    ListenerStruct* lis_ffff;
    ListenerStruct* lis_this;

    memcpy(txd->msg->srcAddr, rxd->msg->dstAddr, 8);
    for (uint16_t i = 0; i < ListenersLen; i++) {
      if ((msg->cmd[0] << 8 | msg->cmd[1]) == 0xFFFF) {
        lis_ffff = Listeners[i];
      }
      if ((msg->cmd[0] << 8 | msg->cmd[1]) == Listeners[i]->port) {
        lis_this = Listeners[i];
      }
    }
    if (lis_ffff->port == 0xFFFF) {
      lis_ffff->fun(msg);
    }
    if (lis_this != lis_ffff) {
      if (lis_this->port != (msg->cmd[0] << 8 | msg->cmd[1]))
        lis_this->fun(msg);
    }
  } else {
    LinkCfgStruct* link;
    link = route->find(rxd->msg->dstAddr);
    if (link) {
      sendOriginal(rxd->buf, rxd->bufLen, link);
    }
  }
}
void Fiip::recv(uint8_t* buf, uint16_t bufLen, LinkCfgStruct* link) {
  INFO("recv:\n");
  if (fiip->rxd->recv(buf, bufLen) == 0) {
    if (fiip->rxd->getMsg()) {
      LinkCfgStruct* oldLink = fiip->route->find(fiip->rxd->msg->srcAddr);
      if (oldLink != NULL) {
        fiip->route->remove(fiip->rxd->msg->srcAddr);
      }
      link->id = new uint8_t[8];
      memcpy(link->id, fiip->rxd->msg->srcAddr, 8);
      fiip->route->update(fiip->rxd->msg->srcAddr, link);
      fiip->solve(fiip->rxd->msg, link);
    } else {
      INFO("crc error\n");
    }
  } else {
    delete link;
  }
}

void Fiip::sendOriginal(uint8_t* data, uint16_t dataLen, LinkCfgStruct* link) {
  if (link->type == linkType_udp) {
    linkUdp->send(data, dataLen, link);
  }
}

void Fiip::send() {
  LinkCfgStruct* link;
  link = route->find(txd->msg->dstAddr);
  if (link) {
    txd->getData();
    INFO("send:\n");
    sendOriginal(txd->buf, txd->bufLen, link);
  } else {
    INFO("no route.\n");
  }
}
void Fiip::broadcast() {
  for (uint16_t i = 0; i < route->linksLen; i++) {
    if (route->links[i]->type == linkType_udp) {
      sendOriginal(txd->buf, txd->bufLen, route->links[i]);
    }
  }
}
void Fiip::start(LinkCfgStruct* link) {
  if (link->type == linkType_udp) {
    linkUdp = new LinkUdp(link);
    linkUdp->recv = this->recv;
    linkUdp->start();
  }
}
void Fiip::connect(uint8_t* id, LinkCfgStruct* link) {
  link->id = new uint8_t[8];
  memcpy(link->id, id, 8);
  route->update(id, link);
  route->fix(id);
}
