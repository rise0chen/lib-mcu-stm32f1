#include "Route.hpp"

void Route::update(uint8_t* id, LinkCfgStruct* link) {
  for (uint16_t i = 0; i < linksLen; i++) {
    if (memcmp(links[i]->id, id, 8) == 0) {
      if (links[i]->status != 0x80) {
        delete links[i];
        links[i] = link;
      }
      return;
    }
  }

  if (linksLen < linksMaxLen) {
    links[linksLen++] = link;
  }
}
void Route::fix(uint8_t* id) {
  for (uint16_t i = 0; i < linksLen; i++) {
    if (memcmp(links[i]->id, id, 8) == 0) {
      links[i]->status = 0x80;
    }
  }
}
void Route::remove(uint8_t* id) {
  for (uint16_t i = 0; i < linksLen; i++) {
    if (memcmp(links[i]->id, id, 8) == 0) {
      delete links[i]->id;
      delete links[i];
      links[i] = links[--linksLen];
    }
  }
}
LinkCfgStruct* Route::find(uint8_t* id) {
  for (uint16_t i = 0; i < linksLen; i++) {
    if (memcmp(links[i]->id, id, 8) == 0) {
      return links[i];
    }
  }
  return NULL;
}
