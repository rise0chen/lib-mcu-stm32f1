#include "transCode.hpp"

unsigned char Utf8ToGbk(const char* pszBufIn, char* pszBufOut){
	unsigned char i_in = 0, len_out = 0;
	unsigned short unicode;
	unsigned int gbkaddr;
	while(pszBufIn[i_in] != 0x00){
		unicode = 0;
		if((pszBufIn[i_in] & 0x80) == 0x00){ //1λ
			pszBufOut[len_out++] = pszBufIn[i_in++];
		}
		else if((pszBufIn[i_in] & 0xE0) == 0xC0){ //2λ
			unicode |= (pszBufIn[i_in++] & 0x1F) << 6;
			unicode |= (pszBufIn[i_in++] & 0x3F);
		}
		else if ((pszBufIn[i_in] & 0xF0) == 0xE0){ //3λ
			unicode |= (pszBufIn[i_in++] & 0x0F) << 12;
			unicode |= (pszBufIn[i_in++] & 0x3F) << 6;
			unicode |= (pszBufIn[i_in++] & 0x3F);
		}
		else{
			return 0;
		}
		
		unsigned char result;
		unsigned int h;
		result=0;
		if(unicode<0xa0) result=1;
		else if(unicode<=0xf7) h=unicode-160;
		else if(unicode<0x2c7) result=1;
		else if(unicode<=0x2c9) h=unicode-160-463;
		else if(unicode<0x2010) result=1;
		else if(unicode<=0x2312) h=unicode-160-463-7494;
		else if(unicode<0x2460) result=1;
		else if(unicode<=0x2642) h=unicode-160-463-7494-333;
		else if(unicode<0x3000) result=1;
		else if(unicode<=0x3017) h=unicode-160-463-7494-333-2493;
		else if(unicode<0x3220) result=1;
		else if(unicode<=0x3229) h=unicode-160-463-7494-333-2493-520;
		else if(unicode<0x4e00) result=1;
		else if(unicode<=0x9b54) h=unicode-160-463-7494-333-2493-520-7126;
		else if(unicode<0x9c7c) result=1;
		else if(unicode<=0x9ce2) h=unicode-160-463-7494-333-2493-520-7126-295;
		else if(unicode<0x9e1f) result=1;
		else if(unicode<=0x9fa0) h=unicode-160-463-7494-333-2493-520-7126-295-316;
		else if(unicode<0xe76c) result=1;
		else if(unicode<=0xe774) h=unicode-160-463-7494-333-2493-520-7126-295-316-18379;
		else if(unicode<0xff00) result=1;
		else if(unicode<=0xff5f) h=unicode-160-463-7494-333-2493-520-7126-295-316-18379-6027;
		else if(unicode<0xffe0) result=1;
		else if(unicode<=0xffe5) h=unicode-160-463-7494-333-2493-520-7126-295-316-18379-6027-128;
		else result=1;
		if(result==0){
			gbkaddr = 0x67d70 + (h<<1);
			*FontRom::CS.O=0;
			FontRom::com.ReadWriteByte(0x03);
			FontRom::com.ReadWriteByte(gbkaddr>>16);
			FontRom::com.ReadWriteByte(gbkaddr>>8);
			FontRom::com.ReadWriteByte(gbkaddr);
			pszBufOut[len_out++] = FontRom::com.ReadWriteByte(0x00);
			pszBufOut[len_out++] = FontRom::com.ReadWriteByte(0x00);
			*FontRom::CS.O=1;
		}
	}
	pszBufOut[len_out] = 0x00;
	return len_out;
}
