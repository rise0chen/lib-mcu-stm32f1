#ifndef __WIFI_H
#define __WIFI_H
#include "sys.hpp"

namespace WIFI{
	extern gpio &Power;
	extern usart &com;
	
	void  On(void);//wifi初始化
	void  Connect(char* name, char* pwd);//连接wifi

	void  TCP_Connect(char* ip, uint16_t port);//连接TCP
	u8    TCP_Send(char* cmd, char* req = "", u8 s=10);//发送TCP数据

	void  TCP_Server(char* ip, uint16_t port);//建立TCP服务器
}
#endif //__WIFI_H
