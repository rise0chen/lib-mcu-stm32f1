#include "wifi.hpp"

namespace WIFI{
	gpio *Power = &gpio_default;
	usart *com = &usart2;

	void On(void){
	//com初始化
		Power->config(P_PPO);
		*Power->O=1;
		delay(1000);
		com->printf("ATE0\r\n");//关闭回显
		delay(200);
		com->printf("AT+CWMODE=1\r\n");//客户端模式
		delay(200);
	}
	void Connect(char* name, char* pwd){
		com->printf("AT+CWJAP=\"%s\",\"%s\"\r\n",name,pwd);
		delay(5000);
	}
	void TCP_Connect(char* ip, uint16_t port){
		com->printf("AT+CIPSTART=\"TCP\",\"%s\",%d\r\n",ip,port);
		delay(1000);
		com->printf("AT+CIPMODE=1\r\n");
		delay(200);
	}
	u8   TCP_Send(char* cmd, char* req, u8 s){
		u8  rt = 0;
		u32 EndTime;
		u32 RunTime = 0;
		com->printf("AT+CIPSEND\r\n");
		delay(200);
		com->printf("%s\r\n",cmd);
		if(req[0]!='\0'){
			EndTime = RunTime + s;
			while(!std::strstr(com->rx.buf,req) && (RunTime<=EndTime));
			if(RunTime<EndTime){rt=1;}
		}else{
			delay(1000);
		}
		com->printf("+++");
		delay(1000);
		return rt;
	}
	void TCP_Server(char* ip, uint16_t port){
		com->printf("AT+CIPMUX=1\r\n");//启用多连接
		delay(200);
		com->printf("AT+CIPAP=\"%s\"\r\n",ip);//AP地址
		delay(200);
		com->printf("AT+CIPSERVER=1,%d\r\n",port);//TCP端口号
		delay(200);
	}
}
