#ifndef __LINK_H
#define __LINK_H
#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

typedef struct {
  uint8_t type;
  uint8_t status;
  uint8_t* address;
  uint32_t port;
  uint8_t* id;
} LinkCfgStruct;  //底层连接配置结构

typedef struct LinkServer {
  uint8_t type;
  LinkCfgStruct* link;
  int fd;
  void (*start)(struct LinkServer* self);
  void (*send)(struct LinkServer* self,
               uint8_t* data,
               uint16_t dataLen,
               LinkCfgStruct*);
  void (*recv)(uint8_t*, uint16_t, LinkCfgStruct*);
} LinkServerStruct;  //底层服务器结构

#ifdef __cplusplus
}
#endif
#endif  //__LINK_H
