#ifndef __LINK_TTY_H
#define __LINK_TTY_H
#include <errno.h>
#include <fcntl.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <termios.h>
#include <unistd.h>
#include "Link.h"

class LinkTty {
#define BUF_LEN 256
 public:
  LinkTty(LinkCfgStruct* link) : link(link){};
  void start();
  void send(uint8_t* data, uint16_t dataLen, LinkCfgStruct* link);
  void (*recv)(uint8_t*, uint16_t, LinkCfgStruct*);

 private:
  int fd;
  LinkCfgStruct* link;
  uint8_t recvBuf[BUF_LEN];
  static void* thread_recvData(void* x);
  int setSpeed(int fd, int speed);
  int setParity(int fd, int databits, int stopbits, int parity);
  int openDev(char* dev);
};

#endif  //__LINK_TTY_H
