#ifndef __STD_H
#define __STD_H
#include <stdint.h>
#include <stdarg.h>
#include <stdio.h>

namespace std{
//<string.h>
	unsigned int strlen(const void *str);
	void* memset(void* dst, int c, unsigned int n);
	void *memcpy(void *dst, const void *src, size_t n=0);
	int memcmp(const void *dst, const void *src, unsigned int n=0);
	char* strstr(char* str1, char* str2 );
	char* strcat(char *dst, const char *src);
//<ctype.h>
	void itoa(int i, char s[], unsigned int n=0);

}
#endif //__STD_H
