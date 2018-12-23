#ifndef __ROUTE_H
#define __ROUTE_H
#include <stdint.h>
#include <string.h>
#include "Link.h"

class Route {
#define linksMaxLen 64

 public:
  LinkCfgStruct* links[linksMaxLen];
  uint16_t linksLen;

  Route(){};
  void update(uint8_t* id, LinkCfgStruct* link);
  void fix(uint8_t* id);
  void remove(uint8_t* id);
  LinkCfgStruct* find(uint8_t* id);

 private:
};

#endif  //__ROUTE_H
