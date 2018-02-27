#include "escape.hpp"

unsigned short Escape(char* bufin, unsigned short lenin, ESC_TypeDef* type, unsigned char typelen, unsigned char way){
	unsigned short i,j,k,outi=0;
	char bufout[512];

	for(i=0;i<lenin;i++){//buf循环i
		for(j=0;j<typelen;j++){//转义结构体循环j
			for(k=0;k<type[j].len[1^way];k++){//转义标识符循环j
				if(bufin[i+k]!=type[j].buf[1^way][k]){
					break;
				}
			}
			if(k==type[j].len[1^way]){
				for(k=0;k<type[j].len[0^way];k++){//转义标识符循环j
					bufout[outi++]=type[j].buf[0^way][k];
				}
				i=i+type[j].len[1^way]-1;
				break;
			}
		}
		if(j==typelen){bufout[outi++]=bufin[i];}
	}
	std::memcpy(bufin, bufout, outi);
	return outi;
}
