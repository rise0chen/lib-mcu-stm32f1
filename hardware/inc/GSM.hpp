#ifndef __GSM_H
#define __GSM_H
#include "sys.hpp"
#include "usart.hpp"

namespace GSM{
	extern Gpio *RST;
	extern Usart *com;
	
	void ON(void);
	void OFF(void);
	
	void TCP_Connect(char* ip, int port);
	void TCP_Send(char* data);
}
#endif
