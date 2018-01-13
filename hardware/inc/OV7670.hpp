#ifndef __OV7670_H
#define __OV7670_H 
#include "sys.hpp"
//摄像头

#pragma  diag_suppress 870/*禁止输出"invalid multibyte character sequence"警告*/

#define FIFO_CS_H()     GPIOD->BSRR =GPIO_Pin_6	  
#define FIFO_CS_L()     GPIOD->BRR  =GPIO_Pin_6	  /*拉低使FIFO输出使能*/

#define FIFO_WRST_H()   GPIOB->BSRR =GPIO_Pin_5	  /*拉高允许FIFO写(数据from摄像头)指针运动 */
#define FIFO_WRST_L()   GPIOB->BRR  =GPIO_Pin_5	  /*拉低使FIFO写(数据from摄像头)指针复位*/

#define FIFO_RRST_H()   GPIOE->BSRR =GPIO_Pin_0	  /*拉高允许FIFO读(数据从FIFO输出)指针运动 */
#define FIFO_RRST_L()   GPIOE->BRR  =GPIO_Pin_0	  /*拉低使FIFO读(数据从FIFO输出)指针复位 */

#define FIFO_RCLK_H()   GPIOE->BSRR =GPIO_Pin_2	  
#define FIFO_RCLK_L()   GPIOE->BRR  =GPIO_Pin_2	  /*FIFO输出数据时钟*/

#define FIFO_WE_H()     GPIOD->BSRR =GPIO_Pin_3	  /*拉高使FIFO写允许*/
#define FIFO_WE_L()     GPIOD->BRR  =GPIO_Pin_3	  

#define OV7670_ID       0x73

extern u8 volatile OV7670_VSYNC;

ErrorStatus OV7670_Init(void);
void VSYNC_Init(void);
void FIFO_GPIO_Configuration(void);
int  OV7670_ReadReg(u8 LCD_Reg,u16 LCD_RegValue);
int  OV7670_WriteReg(u8 LCD_Reg,u16 LCD_RegValue);

#endif //__OV7670_H
