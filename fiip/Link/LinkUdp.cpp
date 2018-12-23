#include "LinkUdp.hpp"
static socklen_t sockLen = sizeof(struct sockaddr);

void LinkUdp::start() {
  fd = socket(AF_INET, SOCK_DGRAM, 0);
  if (fd < 0) {
    perror("socket");
    exit(1);
  }

  /* 将套接字和IP、端口绑定 */
  static struct sockaddr_in* serverCfg = new struct sockaddr_in;
  serverCfg->sin_family = AF_INET;             //使用IPV4地址
  serverCfg->sin_port = link->port;            //端口
  serverCfg->sin_addr.s_addr = link->address;  //地址

  /* 绑定socket */
  if (bind(fd, (struct sockaddr*)serverCfg, sockLen) < 0) {
    perror("bind error:");
    exit(1);
  }
  pthread_t clientHandler;
  pthread_create(&clientHandler, NULL, thread_recvData, this);
}

void LinkUdp::send(uint8_t* data, uint16_t dataLen, LinkCfgStruct* dstLink) {
  static struct sockaddr_in* clientCfg = new struct sockaddr_in;
  clientCfg->sin_family = AF_INET;                //使用IPV4地址
  clientCfg->sin_port = dstLink->port;            //端口
  clientCfg->sin_addr.s_addr = dstLink->address;  //地址
  sendto(fd, data, dataLen, 0, (struct sockaddr*)clientCfg, sockLen);
}

void* LinkUdp::thread_recvData(void* x) {
  LinkUdp* linkUdp = (LinkUdp*)x;
  static struct sockaddr_in* clientCfg = new struct sockaddr_in;
  int recvNum;

  while (1) {
    recvNum = recvfrom(linkUdp->fd, linkUdp->recvBuf, BUF_LEN, 0,
                       (struct sockaddr*)clientCfg, &sockLen);
    if (recvNum < 0) {
      perror("recvfrom error:");
      exit(1);
    }
    LinkCfgStruct* srcLink = new LinkCfgStruct;
    srcLink->type = linkType_udp;
    srcLink->address = clientCfg->sin_addr.s_addr;
    srcLink->port = clientCfg->sin_port;
    linkUdp->recv(linkUdp->recvBuf, recvNum, srcLink);
  }
};
