#ifndef __LINK_H
#define __LINK_H
#include <stdint.h>

typedef enum {
  linkType_log,
  linkType_udp,
  linkType_usart,
  linkType_tty,
} LinkTypeEnum;

typedef struct {
  LinkTypeEnum type;
  uint8_t status;
  uint32_t address;
  uint16_t port;
  uint8_t* id;
} LinkCfgStruct;  //底层连接配置结构

#endif  //__LINK_H
