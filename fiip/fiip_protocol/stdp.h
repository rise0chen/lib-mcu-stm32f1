#ifndef __FIIP_STDP_H
#define __FIIP_STDP_H
#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdlib.h>
#include "../fiip/fiip.h"
#include "../fiip/link/link.h"
#include "./stdpFormat.h"

#define FIIP_PROTOCOL_TYPE_STDP 0x0000
#define STDP_LISTENERS_LEN_MAX 64

typedef struct {
  uint16_t port;
  void (*fun)(Stdp_FieldStruct* msg);
} Stdp_ListenerStruct;  //监听器结构体

void stdp_start(void);
void stdp_addListener(uint16_t port, void (*fun)(Stdp_FieldStruct* msg));

#ifdef __cplusplus
}
#endif
#endif  //__FIIP_STDP_H
