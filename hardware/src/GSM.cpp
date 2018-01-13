#include "GSM.hpp"

void GSM_ON(void)
{
  GPIO_ResetBits(GSM_RESET);
  delay_ms(1000);
  GPIO_SetBits(GSM_RESET);
  delay_ms(1000);
  GPIO_ResetBits(GSM_RESET);
  delay_ms(25000);
  GSM_Send("ATE0\r\n");//关闭回显
  delay_ms(1000);
}
void GSM_OFF(void)
{
  GSM_Send("AT+CIPCLOSE\r\n");//关闭TCP
}
void TCP_Connect(char* ip, int port)
{
  GSM_Send("AT+CGATT=1\r\n");//附着网络
  delay_ms(5000);
  GSM_Send("AT+CGACT=1,1\r\n");//激活网络
  delay_ms(1000);
  GSM_Send("AT+CIPSTART=\"TCP\",\"%s\",%d\r\n",ip,port);//连接TCP
  delay_ms(2000);
}
void TCP_Send(char* data)
{
  GSM_Send("AT+CIPSEND\r\n");
  delay_ms(1000);
  GSM_Send("%s",data);
  while((USART2->SR&0X40)==0){}
  USART_SendData(USART2,0x1a);
  delay_ms(1000);
}










