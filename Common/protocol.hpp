#ifndef __PROTOCOL_H
#define __PROTOCOL_H
#include "sys.hpp"

typedef struct{
	uint8_t SrcAddr[8]; //源地址
	uint8_t DstAddr[8]; //目标地址
	uint8_t Reserve[2]; //预留
	uint8_t TagKey[1];  //加密算法标识
	uint8_t Key[4];     //加密算法密钥
	uint8_t Command[2]; //命令标识
	uint8_t Index[2];   //命令序列号
	uint8_t Len[2];     //消息体长度
	uint8_t Body[256];  //消息体
}protocol_TypeDef;//通信结构体

extern uint16_t  indexCmd;//命令序列号
extern protocol_TypeDef* ToMe;
extern protocol_TypeDef* FromMe;

#endif //__PROTOCOL_H
