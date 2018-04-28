#include "GSM.hpp"

namespace GSM{
	Gpio *RST = &gpio_default;
	Usart *com= &usart2;

	void ON(void){
		*RST->O=0;
		delay_ms(1000);
		*RST->O=1;
		delay_ms(1000);
		*RST->O=0;
		delay_ms(25000);
		com->printf("ATE0\r\n");//关闭回显
		delay_ms(1000);
	}
	void OFF(void){
		com->printf("AT+CIPCLOSE\r\n");//关闭TCP
	}
	void TCP_Connect(char* ip, int port){
		com->printf("AT+CGATT=1\r\n");//附着网络
		delay_ms(5000);
		com->printf("AT+CGACT=1,1\r\n");//激活网络
		delay_ms(1000);
		com->printf("AT+CIPSTART=\"TCP\",\"%s\",%d\r\n",ip,port);//连接TCP
		delay_ms(2000);
	}
	void TCP_Send(char* data){
		com->printf("AT+CIPSEND\r\n");
		delay_ms(1000);
		u8 i=0;
		while(data[i++]!=0x00);
		data[i-1]=0x1a;
		data[i]=0x00;
		com->printf("%s",data);
		delay_ms(1000);
	}
}
