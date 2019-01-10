#ifndef __STD_H
#define __STD_H
#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>

//<string.h>
unsigned int strlen(const void *str);//获取str长度
void* memset(void* dst, int c, unsigned int n);//将dst的前n个字节赋值为c
void* memcpy(void *dst, const void *src, size_t n);//复制src到dst
int   memcmp(const void *dst, const void *src, unsigned int n);//dest>source,返回值>0; dest=source,返回值=0; dest<source ,返回值<0
char* strstr(char* str1, char* str2 );//如果str2是str1的子串,则返回str2在str1中首次出现的地址
char* strcat(char *dst, const char *src);//将src放到dst后面

//<ctype.h>
void itoa(int i, char s[], unsigned int n);//整数型转字符串

#ifdef __cplusplus
}
#endif
#endif //__STD_H
