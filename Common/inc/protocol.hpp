#ifndef __PROTOCOL_H
#define __PROTOCOL_H
#include "sys.hpp"

typedef struct{
	u8 SrcAddr[8]; //源地址
	u8 DstAddr[8]; //目标地址
	u8 Reserve[2]; //预留
	u8 TagKey[1];  //加密算法标识
	u8 Key[4];     //加密算法密钥
	u8 Command[2]; //命令标识
	u8 Index[2];   //命令序列号
	u8 Len[2];     //消息体长度
	u8 Body[256];  //消息体
}protocol_TypeDef;//通信结构体

extern u16  indexCmd;//命令序列号
extern protocol_TypeDef* ToMe;
extern protocol_TypeDef* FromMe;
#endif //__PROTOCOL_H
