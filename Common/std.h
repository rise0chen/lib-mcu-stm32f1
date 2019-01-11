#ifndef __STD_H
#define __STD_H
#ifdef __cplusplus
extern "C" {
#endif

#include <ctype.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*
// <string.h>
//获取str长度
unsigned int strlen(const char* str);

//将dst的前n个字节赋值为c
void* memset(void* dst, int c, unsigned int n);

//复制src到dst
void* memcpy(void* dst, const void* src, size_t n);

// dest>source,返回值>0; dest=source,返回值=0; dest<source ,返回值<0
int memcmp(const void* dst, const void* src, unsigned int n);

//如果str2是str1的子串,则返回str2在str1中首次出现的地址
char* strstr(const char* str1, const char* str2);

//将src放到dst后面
char* strcat(char* dst, const char* src);

// <ctype.h>
//整数型转字符串
void itoa(int i, char* s, int n);
*/

#ifdef __cplusplus
}
#endif
#endif  //__STD_H
