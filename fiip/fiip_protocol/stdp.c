#include "./stdp.h"
static void stdp_solve(Format_FieldStruct* msg, LinkCfgStruct* link);

static Stdp_ListenerStruct* Listeners[STDP_LISTENERS_LEN_MAX];
static uint16_t ListenersLen;

void stdp_start() {
  ProtocolStruct* protocol = (ProtocolStruct*)malloc(sizeof(ProtocolStruct));
  protocol->solve = stdp_solve;
  fiip_addProtocol(FIIP_PROTOCOL_TYPE_STDP, protocol);
}
void stdp_addListener(uint16_t port, void (*fun)(Stdp_FieldStruct* msg)) {
  if (ListenersLen < STDP_LISTENERS_LEN_MAX) {
    Listeners[ListenersLen] =
        (Stdp_ListenerStruct*)malloc(sizeof(Stdp_ListenerStruct));
    Listeners[ListenersLen]->port = port;
    Listeners[ListenersLen]->fun = fun;
    ListenersLen++;
  }
}
void stdp_solve(Format_FieldStruct* fiipMsg, LinkCfgStruct* link) {
  Stdp_FieldStruct* msg = (Stdp_FieldStruct*)fiipMsg;
  Stdp_ListenerStruct* lis_ffff = NULL;
  Stdp_ListenerStruct* lis_this = NULL;

  for (uint16_t i = 0; i < ListenersLen; i++) {
    if (Listeners[i]->port == 0xFFFF) {
      lis_ffff = Listeners[i];
    }
    if (Listeners[i]->port == (msg->cmd[0] << 8 | msg->cmd[1])) {
      lis_this = Listeners[i];
    }
  }
  if (lis_ffff) {
    lis_ffff->fun(msg);
  }
  if (lis_this && lis_this != lis_ffff) {
    if (lis_this->port == (msg->cmd[0] << 8 | msg->cmd[1])) {
      lis_this->fun(msg);
    }
  }
  if (lis_ffff == NULL && lis_this == NULL) {
    printf("no listener.\n");
  }
}
