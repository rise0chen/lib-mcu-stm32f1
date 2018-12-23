#ifndef __FIIP_H
#define __FIIP_H
#include <stdint.h>
#include <stdio.h>
#include "DataFormat/DataFormat.hpp"
#include "Link/LinkUdp.hpp"
#include "Link/Route.hpp"

#ifdef DEBUG
#define INFO(expr) (printf(expr))
#else
#define INFO(expr) ((void)0)
#endif

class Fiip {
#define ListenersMaxLen 64

 public:
  typedef struct {
    uint16_t port;
    void (*fun)(DataFormat::FieldStruct* msg);
  } ListenerStruct;  //监听器结构体

  static uint8_t centerId[8];
  static uint8_t broadId[8];
  DataFormat* rxd;
  DataFormat* txd;

  Fiip() {
    route = new Route();
    rxd = new DataFormat();
    txd = new DataFormat();
  };
  void setId(uint8_t* id, uint8_t* key);
  void addListener(uint16_t port, void (*fun)(DataFormat::FieldStruct* msg));
  void solve(DataFormat::FieldStruct* msg, LinkCfgStruct* link);
  static void recv(uint8_t* buf, uint16_t bufLen, LinkCfgStruct* link);
  void sendOriginal(uint8_t* data, uint16_t dataLen, LinkCfgStruct* link);
  void send();
  void broadcast();
  void start(LinkCfgStruct* link);
  void connect(uint8_t* id, LinkCfgStruct* link);

 private:
  LinkUdp* linkUdp;
  Route* route;
  uint8_t* myId;
  uint8_t* myKey;
  ListenerStruct* Listeners[ListenersMaxLen];
  uint16_t ListenersLen;
};
extern Fiip* fiip;

#endif  //__FIIP_H
