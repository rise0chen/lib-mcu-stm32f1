#ifndef __ROUTE_H
#define __ROUTE_H
#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include "Link.h"

#define LINKS_NUM_MAX 64

extern LinkCfgStruct* route_links[];
extern uint16_t route_linksLen;

int16_t route_update(uint8_t* id, LinkCfgStruct* link);
void route_fix(uint8_t* id);
void route_remove(uint8_t* id);
LinkCfgStruct* route_find(uint8_t* id);

#ifdef __cplusplus
}
#endif
#endif  //__ROUTE_H
