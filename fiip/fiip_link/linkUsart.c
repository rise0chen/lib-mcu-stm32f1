#include "./linkUsart.h"

void linkUsart_start(LinkServerStruct* self);
void linkUsart_send(LinkServerStruct* self,
                  uint8_t* data,
                  uint16_t dataLen,
                  LinkCfgStruct* link);

void startTty(uint8_t* dev, uint32_t speed) {
  LinkCfgStruct* link = (LinkCfgStruct*)malloc(sizeof(LinkCfgStruct));
  LinkServerStruct* linkServer = (LinkServerStruct*)malloc(sizeof(LinkServerStruct));
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

void linkUsart_start(LinkServerStruct* self) {
  LinkCfgStruct* link = self->link;
  int* fd = &self->fd;
  char* dev = (char*)link->address;

  
}

void linkUsart_send(LinkServerStruct* self,
                  uint8_t* data,
                  uint16_t dataLen,
                  LinkCfgStruct* dstLink) {
  int fd = self->fd;

}


void USART1_IRQHandler(void){
  uint8_t recvBuf[1];
  int recvNum=1;
/*
	if(USART1->SR&(1<<5)){
		recvBuf[0]=USART1->DR;
		//to do something
		LinkCfgStruct* srcLink = (LinkCfgStruct*)malloc(sizeof(LinkCfgStruct));
      srcLink->type = linkType_usart;
      srcLink->address = malloc(strlen((char*)self->link->address));
      memcpy(srcLink, self->link->address, strlen((char*)self->link->address));
      srcLink->port = self->link->port;
      self->recv(recvBuf, recvNum, srcLink);
	}
  */
};
