#include "std.hpp"

namespace std{
//<string.h>
	unsigned int strlen(const void *str){//获取str长度
		const char* s=(const char*)str;
		const char *p;

		for(p = s; *s++; );
		return (s - p);
	}
	void* memset(void* dst, int c, unsigned int n){//将dst的前n个字节赋值为c
		unsigned char* d = (unsigned char*) dst;
		
		while (n--) {
			*d++ = (unsigned char) c;
		}
		return dst;
	}
	void *memcpy(void *dst, const void *src, size_t n){//复制src到dst
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
		}
		*d='\0';
		return r;
	}
	int memcmp(const void *dst, const void *src, unsigned int n){//dest>source,返回值>0; dest=source,返回值=0; dest<source ,返回值<0
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
	char* strstr(char* str1, char* str2 ){
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
	char* strcat(char *dst, const char *src){//将src放到dst后面
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
//<ctype.h>
	void itoa(int i, char s[], unsigned int n){
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
}
