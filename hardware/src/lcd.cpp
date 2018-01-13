#include "lcd.hpp"
#include "fonts.hpp"
u8		Direction=0;
vu16	TextColor=0x0000, BackColor=0xFFFF;
u8		Win_X, Win_Y;
u16		Win_W, Win_H;
//PB0--CS		PB1--REST		PB7--RS		PB6--WR		PB5--RD
//改良：
void LCD_delay(u8 time)
{
   u16 i=0;
   while(time--)
   {
      i=1200;
      while(i--);
   }
}

void LCD_CS(u8 state)
{
	if(state)
		GPIO_SetBits(GPIOB, GPIO_Pin_0);
	else
		GPIO_ResetBits(GPIOB, GPIO_Pin_0);
}
void LCD_Reset()
{
	GPIO_ResetBits(GPIOB,GPIO_Pin_1);
	LCD_delay(100);
	GPIO_SetBits(GPIOB,GPIO_Pin_1);
}
void LCD_Write(u16 data)
{
	#ifdef	Bus_16
	GPIOB->ODR = data;
	GPIO_ResetBits(GPIOB, GPIO_Pin_6);
	GPIO_SetBits(GPIOB, GPIO_Pin_6);
	#else
	GPIOB->ODR = (data & 0xff00) | (GPIOB->ODR & 0x00ff);
	GPIO_ResetBits(GPIOB, GPIO_Pin_6);
	GPIO_SetBits(GPIOB, GPIO_Pin_6);
	GPIOB->ODR = (data << 8) | (GPIOB->ODR & 0x00ff);
	GPIO_ResetBits(GPIOB, GPIO_Pin_6);
	GPIO_SetBits(GPIOB, GPIO_Pin_6);
	#endif
}
u16 LCD_Read()
{
	u16 temp;
	LCD_BusIn();
	#ifdef	Bus_16
	GPIO_ResetBits(GPIOB, GPIO_Pin_5);
	temp = GPIOB->IDR;
	GPIO_SetBits(GPIOB, GPIO_Pin_5);
	#else
	GPIO_ResetBits(GPIOB, GPIO_Pin_5);
	temp = ((GPIOB->IDR) & 0xff00);
	GPIO_SetBits(GPIOB, GPIO_Pin_5);
	GPIO_ResetBits(GPIOB, GPIO_Pin_5);
	temp |= (((GPIOB->IDR) & 0xff00)>>8);
	GPIO_SetBits(GPIOB, GPIO_Pin_5);
	#endif
	LCD_BusOut();//读过之后PC 又作为了输出
	return temp;
}

void LCD_WriteReg(u8 reg, u16 RegValue)
{
	//LCD_CS(0);
	GPIO_ResetBits(GPIOB, GPIO_Pin_7);
	LCD_Write(reg);//先送寄存器号
	GPIO_SetBits(GPIOB, GPIO_Pin_7);
	LCD_Write(RegValue);//再送值
	//LCD_CS(1);
}
u16 LCD_ReadReg(u8 reg)
{
	u16 temp;
	//LCD_CS(0);
	GPIO_ResetBits(GPIOB, GPIO_Pin_7);
	LCD_Write(reg);//先送寄存器号
	GPIO_SetBits(GPIOB, GPIO_Pin_7);
	temp = LCD_Read();
	//LCD_CS(1);
	return temp;
}
void LCD_WriteRAM_Prepare(void)
{
	//LCD_CS(0);
	GPIO_ResetBits(GPIOB, GPIO_Pin_7);
	LCD_Write(0x22);
	GPIO_SetBits(GPIOB, GPIO_Pin_7);
	//LCD_CS(1);
}
void LCD_WriteRAM(u16 RGB)
{
	//LCD_CS(0);
	LCD_Write(RGB);//把颜色值发送出去
	//LCD_CS(1);
}
u16 LCD_ReadRAM(void)
{
	u16 temp;

	//LCD_CS(0);
	GPIO_ResetBits(GPIOB, GPIO_Pin_7);
	LCD_Write(0x22);
	GPIO_SetBits(GPIOB, GPIO_Pin_7);
	temp = LCD_Read();//把颜色值读出来
	//LCD_CS(1);

	return temp;
}

void LCD_BusIn(void)
{
	#ifdef	Bus_16
	GPIO_Input_Config(GPIOB,GPIO_Pin_All,GPIO_Mode_IPU);
	#else
	GPIO_Input_Config(GPIOB,0xff00,GPIO_Mode_IPU);
	#endif
}
void LCD_BusOut(void)
{
	#ifdef	Bus_16
	GPIO_Output_Config(GPIOB,GPIO_Pin_All,GPIO_Mode_Out_PP,GPIO_Speed_50MHz);
	#else
	GPIO_Output_Config(GPIOB,0xff00,GPIO_Mode_Out_PP,GPIO_Speed_50MHz);
	#endif
}
void LCD_GPIOConfig(void)
{
	/***** 控制线 *****/
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	GPIO_Output_Config(GPIOB, GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7, GPIO_Mode_Out_PP,GPIO_Speed_50MHz);
	LCD_CS(1);//不片选
	GPIO_SetBits(GPIOB, GPIO_Pin_7);//	数据/命令 线，默认传数据
	GPIO_SetBits(GPIOB, GPIO_Pin_5 | GPIO_Pin_6);//读写时钟
	GPIO_SetBits(GPIOB, GPIO_Pin_1);//	RESER使能

	LCD_BusOut();//数据线可以根据需要配置为输入或者输出
}



void LCD_Init(void)
{
	LCD_GPIOConfig();
	LCD_CS(0);//片选
	LCD_Reset();//复位
	LCD_WriteReg(0x0000,0x0001);//起振
	LCD_delay(20);
	//printf("LCD:%x\n",LCD_ReadReg(0x0000));//9325

	LCD_WriteReg(0x0001,0x0100);//输出驱动控制	SM、SS
	LCD_WriteReg(0x0002,0x0700);//波形控制
	LCD_SetDirection(Direction);//扫描方式
	LCD_WriteReg(0x0004,0x0000);//调整大小
	//显示控制
	LCD_WriteReg(0x0007,0x0133);//显示图像
	LCD_WriteReg(0x0008,0x0207);//上下留边
	LCD_WriteReg(0x0009,0x0000);//扫描周期
	LCD_WriteReg(0x000A,0x0000);//FMARK信号

	LCD_WriteReg(0x000C,0x0000);//RGB显示接口（无法修改？）
	LCD_WriteReg(0x000F,0x0000);//RGB显示接口
	LCD_WriteReg(0x000D,0x0000);//帧标记位
	//电源控制
	LCD_WriteReg(0x0010,0x0000);//睡眠控制
	LCD_WriteReg(0x0011,0x0007);//工作频率
	LCD_WriteReg(0x0012,0x0000);//vregiout
	LCD_WriteReg(0x0013,0x0000);//Vcom频率
	LCD_WriteReg(0x0029,0x0018);//电压VcomH

	LCD_WriteReg(0x0010,0x1290);
	LCD_WriteReg(0x0011,0x0227);
	LCD_WriteReg(0x0012,0x001d);
	LCD_WriteReg(0x0013,0x1500);

	LCD_WriteReg(0x002B,0x000D);//帧速率和颜色

	//伽马控制
	LCD_WriteReg(0x0030,0x0004);
	LCD_WriteReg(0x0031,0x0307);
	LCD_WriteReg(0x0032,0x0002);//0006
	LCD_WriteReg(0x0035,0x0206);
	LCD_WriteReg(0x0036,0x0408);
	LCD_WriteReg(0x0037,0x0507);
	LCD_WriteReg(0x0038,0x0204);//0200
	LCD_WriteReg(0x0039,0x0707);
	LCD_WriteReg(0x003C,0x0405);//0504
	LCD_WriteReg(0x003D,0x0F02);

	//ram
	LCD_WriteReg(0x0050,0x0000);
	LCD_WriteReg(0x0051,0x00EF);
	LCD_WriteReg(0x0052,0x0000);
	LCD_WriteReg(0x0053,0x013F);
	//门扫描控制
	LCD_WriteReg(0x0060,0xA700);
	LCD_WriteReg(0x0061,0x0001);
	LCD_WriteReg(0x006A,0x0000);
	//部分图像1
	LCD_WriteReg(0x0080,0x0000);
	LCD_WriteReg(0x0081,0x0000);
	LCD_WriteReg(0x0082,0x0000);
	//部分图像2
	LCD_WriteReg(0x0083,0x0000);
	LCD_WriteReg(0x0084,0x0000);
	LCD_WriteReg(0x0085,0x0000);
	//屏接口
	LCD_WriteReg(0x0090,0x0010);
	LCD_WriteReg(0x0092,0x0600);
	LCD_WriteReg(0x0093,0x0003);
	LCD_WriteReg(0x0095,0x0110);
	LCD_WriteReg(0x0097,0x0000);
	LCD_WriteReg(0x0098,0x0000);
	//RA1~RA5	OTP编程

	LCD_Clear(BackColor);
}
void LCD_SetDirection(u8 d)
{
	//AM(3)	ID1(5)	ID0(4)	FUNCATION
	//0			0				0				R->L D->U	2		0x1000
	//0			0				1				L->R D->U			0x1010
	//0			1				0				R->L U->D			0x1020
	//0			1				1				L->R U->D	0		0x1030
	//1			0				0				D->U R->L			0x1008
	//1			0				1				D->U L->R	3		0x1018
	//1			1				0				U->D R->L	1		0x1028
	//1			1				1				U->D L->R			0x1038
	switch(d)
	{
		case 0:
			Direction=0;
			LCD_WriteReg(0x0003,0x1030);
		break;
		case 1:
			Direction=1;
			LCD_WriteReg(0x0003,0x1028);
		break;
		case 2:
			Direction=2;
			LCD_WriteReg(0x0003,0x1000);
		break;
		case 3:
			Direction=3;
			LCD_WriteReg(0x0003,0x1018);
		break;
	}
}

void LCD_SetCursor(u16 Xpos, u16 Ypos)
{
	switch(Direction)
	{
		case 0:
			LCD_WriteReg(0x20, Xpos);
			LCD_WriteReg(0x21, Ypos);
		break;
		case 1:
			LCD_WriteReg(0x20, LCD_W - Ypos-1);
			LCD_WriteReg(0x21, Xpos);
		break;
		case 2:
			LCD_WriteReg(0x20, LCD_W - Xpos-1);
			LCD_WriteReg(0x21, LCD_H - Ypos-1);
		break;
		case 3:
			LCD_WriteReg(0x20, Ypos);
			LCD_WriteReg(0x21, LCD_H - Xpos-1);
		break;
	}
}
void LCD_SetWindow(u16 x,u16 y,u16 w,u16 h)
{
	Win_X=x;
	Win_Y=y;
	Win_W=w;
	Win_H=h;
	switch(Direction)
	{
		case 0:
			LCD_WriteReg(0x50,x);//X起始
			LCD_WriteReg(0x52,y);//Y起始
			LCD_WriteReg(0x51,x+w-1);//X结束
			LCD_WriteReg(0x53,y+h-1);//Y结束
		break;
		case 1:
			LCD_WriteReg(0x50,LCD_W-y-h);
			LCD_WriteReg(0x52,x);
			LCD_WriteReg(0x51,LCD_W-y-1);
			LCD_WriteReg(0x53,x+w-1);
		break;
		case 2:
			LCD_WriteReg(0x50,LCD_W-x-w);
			LCD_WriteReg(0x52,LCD_H-y-h);
			LCD_WriteReg(0x51,LCD_W-x-1);
			LCD_WriteReg(0x53,LCD_H-y-1);
		break;
		case 3:
			LCD_WriteReg(0x50,y);
			LCD_WriteReg(0x52,LCD_H-x-w);
			LCD_WriteReg(0x51,y+h-1);
			LCD_WriteReg(0x53,LCD_H-x-1);
		break;
	}
}


void LCD_Display(u8 state)
{
	if(state)
		LCD_WriteReg(0x07, 0x0133);//显示
	else
		LCD_WriteReg(0x07, 0x0000);//不显示
}
void LCD_Clear(u16 Color)
{
	u16 x=0, y=0;
	LCD_SetCursor(0,0);//设置光标位置
	LCD_WriteRAM_Prepare();//开始写入GRAM
	LCD_WriteRAM(Color);
	for(x=0;x<Win_W;x++)//240
	{
		for(y=0;y<Win_H;y++)//320
		{
			#ifdef	Bus_16
			GPIO_ResetBits(GPIOB, GPIO_Pin_6);
			GPIO_SetBits(GPIOB, GPIO_Pin_6);
			#else
			GPIO_ResetBits(GPIOB, GPIO_Pin_6);
			GPIO_SetBits(GPIOB, GPIO_Pin_6);
			GPIO_ResetBits(GPIOB, GPIO_Pin_6);
			GPIO_SetBits(GPIOB, GPIO_Pin_6);
			#endif
		}
	}
}


void LCD_DrawPoint(u16 x,u16 y)
{
	LCD_SetCursor(x,y);
	LCD_WriteRAM_Prepare();
	LCD_WriteRAM(TextColor);
}
void LCD_DrawBigPoint(u16 x,u16 y)
{
	u8 i,j;
	x=x-1;
	y=y-1;
	for(i=0;i<3;i++)
	{
		LCD_SetCursor(x,y);
		LCD_WriteRAM_Prepare();
		for(j=0;j<3;j++)
			LCD_WriteRAM(TextColor);
		y++;
	}
}
void LCD_DrawLine(u16 Xpos, u16 Ypos, u16 Length, u8 d)
{
	u16 i = 0;
	if(d)//垂直
	{
		for(i = 0; i < Length; i++)
		{
			LCD_SetCursor(Xpos, Ypos);
			LCD_WriteRAM_Prepare();
			LCD_WriteRAM(TextColor);
			Ypos++;
		}
	}
	else//水平
	{
		LCD_SetCursor(Xpos, Ypos);
		LCD_WriteRAM_Prepare();
		for(i = 0; i < Length; i++)
		{
			LCD_WriteRAM(TextColor);
		}
	}
}
void LCD_DrawRect(u16 Xpos, u16 Ypos, u16 Width, u16 Height)
{
	LCD_DrawLine(Xpos, Ypos, Width, 0);
	LCD_DrawLine(Xpos, (Ypos + Height), Width, 0);

	LCD_DrawLine(Xpos, Ypos, Height, 1);
	LCD_DrawLine((Xpos + Width), Ypos, Height, 1);
}
void LCD_DrawChar(u16 Xpos, u16 Ypos, uc16 *c)
{
	u8 index = 0, i = 0;

	for(index = 0; index < 24; index++)
	{
		LCD_SetCursor(Xpos, Ypos);
		LCD_WriteRAM_Prepare();
		for(i = 0; i < 16; i++)
		{
			if((c[index] & (1 << i)) == 0x00)
			{
				LCD_WriteRAM(BackColor);
			}
			else
			{
				LCD_WriteRAM(TextColor);
			}
		}
		Ypos++;
	}
}
void LCD_DisplayChar(u16 Line, u16 Column, u8 Ascii)
{
	Ascii -= 32;
	LCD_DrawChar(Column, Line, &Font16x24.table[Ascii * 24]);
}
void LCD_DisplayString(u16 Line, u8 *ptr)
{
	u16 column = 0, max;
	if(Direction==0 | Direction==2)
		max=LCD_W;
	else
		max=LCD_H;
	while ((*ptr != 0) && (column < max))
	{
		LCD_DisplayChar(Line, column, *ptr);
		column += 16;
		ptr++;
	}
}
void LCD_ClearLine(u16 Line)
{
	LCD_DisplayString(Line, "                    ");
}
void LCD_DrawMonoPict(u16 x, u16 y, uc8 *Pict)
{
	u16 w,h,index = 6, i = 0;
	w=Pict[2];//宽
	h=Pict[4];//高
	LCD_SetWindow(x,y,w,h);
	LCD_SetCursor(x, y);
	LCD_WriteRAM_Prepare();
	for(index = 6; index < w*h/8; index++)
	{
		for(i = 0; i < 8; i++)
		{
			if((Pict[index] & (1 << i)) == 0x00)
			{
				LCD_WriteRAM(BackColor);
			}
			else
			{
				LCD_WriteRAM(TextColor);
			}
		}
	}
}
void LCD_WriteBMP(u32 BmpAddress)
{
	u32 index = 0, size = 0;
	size = *(vu16 *) (BmpAddress + 2);
	size |= (*(vu16 *) (BmpAddress + 4)) << 16;
	index = *(vu16 *) (BmpAddress + 10);
	index |= (*(vu16 *) (BmpAddress + 12)) << 16;
	size = (size - index)/2;
	BmpAddress += index;
	LCD_WriteReg(0x03, 0x1008);//?
	LCD_WriteRAM_Prepare();
	for(index = 0; index < size; index++)
	{
		LCD_WriteRAM(*(vu16 *)BmpAddress);
		BmpAddress += 2;
	}
	LCD_WriteReg(0x03, 0x1018);//？？？
}
void LCD_DrawPicture(const u8* picture)
{
	u32 index;
	LCD_SetWindow(0,0,240,240);//显示240*240
	LCD_WriteRAM_Prepare();
	for(index = 0; index < 240 * 240 ; index++)
	{
		LCD_WriteRAM(picture[2*index+1]<<8|picture[2*index]);//低位在前
	}
}
