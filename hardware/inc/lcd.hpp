#ifndef __LCD_H
#define __LCD_H
#include "sys.hpp"

//定义LCD的尺寸	
#define LCD_W 240
#define LCD_H 320
//#define Bus_16//16位数据模式
extern u8		Direction;//手持方向
extern vu16	TextColor, BackColor;//字体、背景颜色
extern u8		Win_X, Win_Y;//窗口起始坐标
extern u16	Win_W, Win_H;//窗口宽、高


void LCD_delay(u8 time);//延时0.1毫秒
void LCD_CS(u8 state);//片选
void LCD_Reset(void);//复位
void LCD_Write(u16 data);//写
u16	LCD_Read(void);//读
void LCD_WriteReg(u8 reg, u16 RegValue);//写寄存器
u16 LCD_ReadReg(u8 reg);//读寄存器
void LCD_WriteRAM_Prepare(void);//写显存准备
void LCD_WriteRAM(u16 RGB);//写显存
u16 LCD_ReadRAM(void);//读显存
void LCD_BusIn(void);//数据线输入
void LCD_BusOut(void);//数据线输出
void LCD_GPIOConfig(void);//配置引脚
void LCD_Init(void);//初始化
void LCD_SetDirection(u8 d);//扫描方式
void LCD_SetCursor(u16 Xpos, u16 Ypos);//光标位置
void LCD_SetWindow(u16 x,u16 y,u16 w,u16 h);//操作区域
void LCD_Display(u8 state);//显示画面
void LCD_Clear(u16 Color);//清屏

void LCD_DrawPoint(u16 x,u16 y);//画点
void LCD_DrawBigPoint(u16 x,u16 y);//画大点
void LCD_DrawLine(u16 Xpos, u16 Ypos, u16 Length, u8 Direction);//线
void LCD_DrawRect(u16 Xpos, u16 Ypos, u16 Width, u16 Height);//矩形框
void LCD_DrawChar(u16 Xpos, u16 Ypos, uc16 *c);//画16*24文字
void LCD_DisplayChar(u16 Line, u16 Column, u8 Ascii);//画Ascii码
void LCD_DisplayString(u16 Line, u8 *ptr);//某一行输出字符串
void LCD_ClearLine(u16 Line);//某一行输出空
void LCD_DrawMonoPict(u16 x, u16 y, uc8 *Pict);//黑白照片
void LCD_WriteBMP(u32 BmpAddress);//BMP照片
void LCD_DrawPicture(const u8* picture);//16色的照片 每两个字节是一个像素的颜色值



#define Line0          0
#define Line1          24
#define Line2          48
#define Line3          72
#define Line4          96
#define Line5          120
#define Line6          144
#define Line7          168
#define Line8          192
#define Line9          216
#define Line10         240
#define Line11         264
#define Line12         288

/* LCD color */
#define Red							0xF800
#define Green						0x07E0
#define Blue						0x001F
#define Blue2						0x051F

#define White						0xFFFF
#define Black						0x0000
#define Grey						0xF7DE
#define Magenta					0xF81F//品红
#define Cyan						0x7FFF//蓝绿色
#define Yellow					0xFFE0
#define Brown						0XBC40//棕色
#define Brred						0XFC07//棕红色
#define Gray						0X8430//灰色
#define DarkBlue				0X01CF	//深蓝色
#define LightBlue				0X7D7C	//浅蓝色
#define GrayBlue				0X5458 //灰蓝色
#define LightGreen			0X841F //浅绿色
#define LightGray				0XC618 //浅灰色(PANNEL),窗体背景色

#endif //__LCD_H
