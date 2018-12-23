#ifndef __LINK_UDP_H
#define __LINK_UDP_H
#include <errno.h>
#include <netinet/in.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include "Link.h"

class LinkUdp {
#define BUF_LEN 256
 public:
  LinkUdp(LinkCfgStruct* link) : link(link){};
  void start();
  void send(uint8_t* data, uint16_t dataLen, LinkCfgStruct* link);
  void (*recv)(uint8_t*, uint16_t, LinkCfgStruct*);

 private:
  int fd;
  LinkCfgStruct* link;
  uint8_t recvBuf[BUF_LEN];
  static void* thread_recvData(void* x);
};

#endif  //__LINK_UDP_H
