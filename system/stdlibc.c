#include "sys.h"
#include <stdio.h>
/*****************************************************
*function:	写字符文件函数
*param1:	输出的字符
*param2:	文件指针
*return:	输出字符的ASCII码
******************************************************/
_C int fputc(int ch, FILE *f) {
  while((USART1->SR & 0x0040) == 0);
  USART1->DR = (ch & (uint16_t)0x01FF);
	return ch;
}

/********** 禁用半主机模式 **********/
//#pragma import(__use_no_semihosting)
/*
*/
struct __FILE {
	int a;
};
//FILE __stdout;
void _sys_exit(int x){}
