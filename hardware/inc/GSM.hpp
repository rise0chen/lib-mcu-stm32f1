#ifndef __GSM_H
#define __GSM_H
#include "sys.hpp"

#define GSM_RESET   GPIOA, GPIO_Pin_6

#define GSM_Send    USART2_Send
void GSM_ON(void);
void GSM_OFF(void);
void TCP_Connect(char* ip, int port);
void TCP_Send(char* data);
#endif
