#ifndef __SCCB_H
#define __SCCB_H
#include "sys.hpp"
//摄像头总线

#define SCL_H         GPIO_SetBits(GPIOC , GPIO_Pin_6) 
#define SCL_L         GPIO_ResetBits(GPIOC , GPIO_Pin_6) 
   
#define SDA_H         GPIO_SetBits(GPIOC , GPIO_Pin_7) 
#define SDA_L         GPIO_ResetBits(GPIOC , GPIO_Pin_7) 

#define SCL_read      GPIO_ReadInputDataBit(GPIOC , GPIO_Pin_6) 
#define SDA_read      GPIO_ReadInputDataBit(GPIOC , GPIO_Pin_7) 

#define ADDR_OV7725   0x42

void SCCB_GPIO_Configuration(void);
int SCCB_WriteByte( u16 WriteAddress , u8 SendByte);
int SCCB_ReadByte(u8* pBuffer,   u8 ReadAddress);

#endif //__SCCB_H
