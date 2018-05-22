/*************************************************
Copyright (C), 2018-2028, Crise Tech. Co., Ltd.
File name: std.cpp
Author: rise0chen
Version: 1.0
Date: 2018.4.26
Description: 重写标准函数库
Usage: 
History: 
	rise0chen   2018.4.26   编写注释
*************************************************/
#include "std.hpp"

/********--------      <string.h>      --------********/
/*************************************************
Function: std::strlen
Description: 字符串长度 以0x00结尾
Input: str  字符串指针
Return: void
*************************************************/
unsigned int std::strlen(const void *str){
	const char* s=(const char*)str;
	const char *p;

	for(p = s; *s++; );
	return (s - p);
}

/*************************************************
Function: std::memset
Description: 批量设置内存的字节
Input: 
	dst   变量地址
	c     字节
	n     字节长度
Return: void
*************************************************/
void* std::memset(void* dst, int c, unsigned int n){
	unsigned char* d = (unsigned char*) dst;
	
	while (n--) {
		*d++ = (unsigned char) c;
	}
	return dst;
}

/*************************************************
Function: std::memcpy
Description: 复制内存到其他内存
Input: 
	dst   目标地址
	src   源地址
	n     字节长度 0代表字符串
Return: void
*************************************************/
void* std::memcpy(void *dst, const void *src, size_t n){
	unsigned char* d = (unsigned char*) dst;
	unsigned char* s = (unsigned char*) src;
	unsigned char* r=d;
	
	if(n){
		while(n--){
			*d++=*s++;
		}
	}else{
		while(*s){
			*d++=*s++;
		}
		*d='\0';
	}
	return r;
}

/*************************************************
Function: std::memcmp
Description: 比较内存是否相同
Input: 
	dst   待比较
	src   期望数据
	n     字节长度 0代表字符串
Return: 0相同  非0不相同
*************************************************/
int std::memcmp(const void *dst, const void *src, unsigned int n){
	unsigned char* d = (unsigned char*) dst;
	unsigned char* s = (unsigned char*) src;
	
	if(n){
		while(n--){
			if((*d==*s) && n){
				d++;
				s++;
			}else{break;}
		}
	}else{
		while (*d && *s && (*d == *s)){
			d++;
			s++;
		}
	}
	return *d - *s;
}

/*************************************************
Function: std::strstr
Description: 比较str2是否是str1的子串
Input: 
	str1  长字符串
	str2  短字符串
Return: void
*************************************************/
char* std::strstr(char* str1, char* str2 ){
	char *p = NULL;
	char *q = NULL;
	if(str1 == NULL || str2 == NULL) return NULL;
	
	for (int i=0; str1[i] != '\0'; ++i){
		p = &str1[i];
		q = str2;
		
		while (*q != '\0' && *q == *p){
				++p;
				++q;
		}
		if ('\0' == *q){
			return &str1[i];
		}
	}
	return NULL;
}

/*************************************************
Function: std::strcat
Description: 将src放到dst后面
Input: 
	dst   前面字符串
	src   后面字符串
Return: void
*************************************************/
char* std::strcat(char *dst, const char *src){
	char * r = dst;
	
	while(*dst){//若使用while(*dst++)，则会出错
		dst++;
	}
	while(*src){
		*dst++ = *src++;
	}
	*dst = '\0';
	return r;
}


/********--------      <ctype.h>      --------********/
/*************************************************
Function: std::itoa
Description: 将整数型转换为10进制字符串
Input: 
	i     前面字符串
	s     后面字符串
	n     字符串长度
Return: void
*************************************************/
void std::itoa(int i, char s[], unsigned int n){
	int bak=i;
	char a=0;
	while(bak /= 10){
		a++;
	}
	if(n<a){
		n=(++a);
	}else{a=n;}
	s[a]='\0';
	while(a--){
		s[--n] = 0x30+(i%10);
		i /= 10;
	}
	while(n--){
		s[n]='0';
	}
}

