#include "wifi.hpp"

namespace wifi{
	gpio Power=gpio(PG, 7);
	usart& com=usart2;

	void On(void){
	//com初始化
		Power.Config(P_PPO);
		*Power.O=1;
		delay(1000);
		com.printf("ATE0\r\n");//关闭回显
		delay(200);
		com.printf("AT+CWMODE=1\r\n");//客户端模式
		delay(200);
	}
	void Connect(char* name, char* pwd){
		com.printf("AT+CWJAP=\"%s\",\"%s\"\r\n",name,pwd);
		delay(5000);
	}
	void TCP_Connect(char* ip, uint16_t port){
		com.printf("AT+CIPSTART=\"TCP\",\"%s\",%d\r\n",ip,port);
		delay(1000);
		com.printf("AT+CIPMODE=1\r\n");
		delay(200);
	}
	u8   TCP_Send(char* cmd, char* req, u8 s){
		u8  rt;
		com.printf("AT+CIPSEND\r\n");
		delay(200);
		com.printf("%s\r\n",cmd);
		if(req[0]!='\0'){
			delay_ms(s*1000, 0);
			while(!std::strstr(com.RX_BUF,req) && delay_n);
			rt = delay_n ? 0 : 1;
		}else{
			delay(1000);
			rt = 0;
		}
		com.printf("+++");
		delay(1000);
		return rt;
	}
	void TCP_Server(char* ip, uint16_t port){
		com.printf("AT+CIPMUX=1\r\n");//启用多连接
		delay(200);
		com.printf("AT+CIPAP=\"%s\"\r\n",ip);//AP地址
		delay(200);
		com.printf("AT+CIPSERVER=1,%d\r\n",port);//TCP端口号
		delay(200);
	}
}
