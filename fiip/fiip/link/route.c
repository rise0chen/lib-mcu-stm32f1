#include "route.h"
LinkCfgStruct* route_links[LINKS_NUM_MAX];
uint16_t route_linksLen = 0;

int16_t route_update(uint8_t* id, LinkCfgStruct* link) {
  for (uint16_t i = 0; i < route_linksLen; i++) {
    if (memcmp(route_links[i]->id, id, 8) == 0) {
      if (route_links[i]->status != 0x80) {
        link->id = route_links[i]->id;
        free(route_links[i]->address);
        free(route_links[i]);
        route_links[i] = link;
        return i;
      } else {
        free(link);
        return -1;
      }
    }
  }

  if (route_linksLen < LINKS_NUM_MAX) {
    link->id = (uint8_t*)malloc(8);
    memcpy(link->id, id, 8);
    route_links[route_linksLen++] = link;
    return route_linksLen - 1;
  }
  return -2;
}
void route_fix(uint8_t* id) {
  for (uint16_t i = 0; i < route_linksLen; i++) {
    if (memcmp(route_links[i]->id, id, 8) == 0) {
      route_links[i]->status = 0x80;
    }
  }
}
void route_remove(uint8_t* id) {
  for (uint16_t i = 0; i < route_linksLen; i++) {
    if (memcmp(route_links[i]->id, id, 8) == 0) {
      free(route_links[i]->id);
      free(route_links[i]->address);
      free(route_links[i]);
      route_links[i] = route_links[--route_linksLen];
    }
  }
}
LinkCfgStruct* route_find(uint8_t* id) {
  for (uint16_t i = 0; i < route_linksLen; i++) {
    if (memcmp(route_links[i]->id, id, 8) == 0) {
      return route_links[i];
    }
  }
  return NULL;
}
