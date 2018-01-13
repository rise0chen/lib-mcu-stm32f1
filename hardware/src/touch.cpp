#include "touch.hpp"
#include "lcd.hpp"
#include "spi.hpp"

#define Threshold 2//差值门限
static u16 Touch_AD_X, Touch_AD_Y;//触摸坐标AD值
u16 Touch_X, Touch_Y;//触摸坐标
static double touch_para[7];//校准参数：0~5:An~Fn; 6:Divider
static double aa1=0,bb1=0,cc1=0,aa2=0,bb2=0,cc2=0;//校准值
u16 CalibrateCoord[4][2]=
{
	{45 , 45 },//校准坐标
	{45 , 270},
	{190, 190},
	{210, 280}
};




static void DelayUS(vu32 cnt)
{
	uint16_t i;
	for(i = 0;i<cnt;i++)
	{
		uint8_t us = 12; /* 设置值为12，大约延1微秒 */
		while (us--);
	}
}


void Touch_init(void)
{
    SPI_Config(SPI1, 0);
//GPIO_Output_Config(GPIOA,GPIO_Pin_3,GPIO_Mode_Out_PP,GPIO_Speed_50MHz);
    EXTI_Config(GPIO_A,Pin8,EXTI_Trigger_Falling);
    SPI_CS(1);
}


static u16 Touch_ReadAD(void)
{
	u16 buf,temp;

	temp=SPI_SendByte(0x00);
	buf=temp<<8;
	DelayUS(1);
	temp=SPI_SendByte(0x00);
	buf |= temp;
	buf >>= 3;
	buf &= 0xfff;
	return buf;
}
static void Touch_ReadAdXY()
{
	SPI_CS(0);
	DelayUS(1);
	SPI_SendByte(0xd0);//X选择控制字
	DelayUS(1);
	Touch_AD_X=Touch_ReadAD();

	DelayUS(1);
	SPI_SendByte(0x90);//Y选择控制字
	DelayUS(1);
	Touch_AD_Y=Touch_ReadAD();
	SPI_CS(1);
}



u8 Touch_GetXY_Real()
{
	u16 buffer[2][9]={{0},{0}};//坐标X和Y进行9次采样
	int m0,m1,m2,temp[3];
	uint8_t count=0;

	while(!Touch_In && count<9)//用户点击触摸屏 并且 count<9
	{
		Touch_ReadAdXY();
		buffer[0][count]=Touch_AD_X;
		buffer[1][count]=Touch_AD_Y;
		count++;
	}
	if(count==9)//如果成功采样9次,进行滤波
	{
		/* 为减少运算量,分别分3组取平均值 */
		temp[0]=(buffer[0][0]+buffer[0][1]+buffer[0][2])/3;
		temp[1]=(buffer[0][3]+buffer[0][4]+buffer[0][5])/3;
		temp[2]=(buffer[0][6]+buffer[0][7]+buffer[0][8])/3;

		/* 计算3组数据的差值 */
		m0=temp[0]-temp[1];
		m1=temp[1]-temp[2];
		m2=temp[2]-temp[0];

		/* 对上述差值取绝对值 */
		m0=m0>0?m0:(-m0);
		m1=m1>0?m1:(-m1);
		m2=m2>0?m2:(-m2);

		if( m0>Threshold  &&  m1>Threshold  &&  m2>Threshold )//判断绝对差值是否都超过差值门限
			return 1;

		/* 计算它们的平均值，同时赋值给screen */
		if(m0<m1)
		{
			if(m2<m0)
				Touch_AD_X=(temp[0]+temp[2])/2;
			else
				Touch_AD_X=(temp[0]+temp[1])/2;
		}
		else if(m2<m1)
			Touch_AD_X=(temp[0]+temp[2])/2;
		else
			Touch_AD_X=(temp[1]+temp[2])/2;

		/* 同上 计算Y的平均值 */
		temp[0]=(buffer[1][0]+buffer[1][1]+buffer[1][2])/3;
		temp[1]=(buffer[1][3]+buffer[1][4]+buffer[1][5])/3;
		temp[2]=(buffer[1][6]+buffer[1][7]+buffer[1][8])/3;
		m0=temp[0]-temp[1];
		m1=temp[1]-temp[2];
		m2=temp[2]-temp[0];
		m0=m0>0?m0:(-m0);
		m1=m1>0?m1:(-m1);
		m2=m2>0?m2:(-m2);
		if(m0>Threshold&&m1>Threshold&&m2>Threshold)
			return 1;

		if(m0<m1)
		{
			if(m2<m0)
				Touch_AD_Y=(temp[0]+temp[2])/2;
			else
				Touch_AD_Y=(temp[0]+temp[1])/2;
		}
		else if(m2<m1)
			Touch_AD_Y=(temp[0]+temp[2])/2;
		else
			Touch_AD_Y=(temp[1]+temp[2])/2;
		return 0;//成功
	}
	return 1;//失败
}


u8 Touch_GetXY()
{
	u16 buffer[2][10]={{0},{0}};  /*坐标X和Y进行多次采样*/
	u16 min_x,max_x;
	u16 min_y,max_y;
	u8	i=0;
	u8 count=0;

	while(!Touch_In && count<10)//用户点击触摸屏 并且 count<9
	{
		Touch_ReadAdXY();
		buffer[0][count]=Touch_AD_X;
		buffer[1][count]=Touch_AD_Y;
		count++;
	}
	/*如果成功采样10个样本*/
	if(count ==10)		 
	{
		max_x=min_x=buffer[0][0];
		max_y=min_y=buffer[1][0];
		for(i=1; i<10; i++)
		{
			if(buffer[0][i]<min_x)
				min_x=buffer[0][i];
			else
				max_x = buffer[0][i];
		}

		for(i=1; i<10; i++)
		{
			if(buffer[1][i]<min_y)
				min_y=buffer[1][i];
			else
				max_y = buffer[1][i];
		}

		/*去除最小值和最大值之后求平均值*/
		Touch_AD_X=(buffer[0][0]+buffer[0][1]+buffer[0][2]+buffer[0][3]+buffer[0][4]+buffer[0][5]+buffer[0][6]+buffer[0][7]+buffer[0][8]+buffer[0][9]-min_x-max_x)>>3;
		Touch_AD_Y=(buffer[1][0]+buffer[1][1]+buffer[1][2]+buffer[1][3]+buffer[1][4]+buffer[1][5]+buffer[1][6]+buffer[1][7]+buffer[1][8]+buffer[1][9]-min_y-max_y)>>3;
		return 0;//成功
	}
	return 1;//失败
}


static u8 Touch_CalcPara(u16 Coord[4][2],u16 Sample[4][2])
{
	/* K＝(X0－X2) (Y1－Y2)－(X1－X2) (Y0－Y2) */
	touch_para[6] = ((Sample[0][0] - Sample[2][0]) * (Sample[1][1] - Sample[2][1])) -
	((Sample[1][0] - Sample[2][0]) * (Sample[0][1] - Sample[2][1])) ;

	if(touch_para[6] == 0 )
	{
		return 1;//失败
	}
	else
	{
		/* A＝((XD0－XD2) (Y1－Y2)－(XD1－XD2) (Y0－Y2))／K	*/
		touch_para[0] = ((Coord[0][0] - Coord[2][0]) * (Sample[1][1] - Sample[2][1])) - ((Coord[1][0] - Coord[2][0]) * (Sample[0][1] - Sample[2][1]));
		/* B＝((X0－X2) (XD1－XD2)－(XD0－XD2) (X1－X2))／K	*/
		touch_para[1] = ((Sample[0][0] - Sample[2][0]) * (Coord[1][0] - Coord[2][0])) - ((Coord[0][0] - Coord[2][0]) * (Sample[1][0] - Sample[2][0]));
		/* C＝(Y0(X2XD1－X1XD2)+Y1(X0XD2－X2XD0)+Y2(X1XD0－X0XD1))／K */
		touch_para[2] = (Sample[2][0] * Coord[1][0] - Sample[1][0] * Coord[2][0]) * Sample[0][1] +(Sample[0][0] * Coord[2][0] - Sample[2][0] * Coord[0][0]) * Sample[1][1] +(Sample[1][0] * Coord[0][0] - Sample[0][0] * Coord[1][0]) * Sample[2][1] ;
		/* D＝((YD0－YD2) (Y1－Y2)－(YD1－YD2) (Y0－Y2))／K	*/
		touch_para[3] = ((Coord[0][1] - Coord[2][1]) * (Sample[1][1] - Sample[2][1])) - ((Coord[1][1] - Coord[2][1]) * (Sample[0][1] - Sample[2][1])) ;
		/* E＝((X0－X2) (YD1－YD2)－(YD0－YD2) (X1－X2))／K	*/
		touch_para[4] = ((Sample[0][0] - Sample[2][0]) * (Coord[1][1] - Coord[2][1])) - ((Coord[0][1] - Coord[2][1]) * (Sample[1][0] - Sample[2][0])) ;
		/* F＝(Y0(X2YD1－X1YD2)+Y1(X0YD2－X2YD0)+Y2(X1YD0－X0YD1))／K */
		touch_para[5] = (Sample[2][0] * Coord[1][1] - Sample[1][0] * Coord[2][1]) * Sample[0][1] +(Sample[0][0] * Coord[2][1] - Sample[2][0] * Coord[0][1]) * Sample[1][1] +(Sample[1][0] * Coord[0][1] - Sample[0][0] * Coord[1][1]) * Sample[2][1];
	}
	return 0;//成功
}


u8 Touch_GetPoint()
{
	Touch_GetXY();
	if(touch_para[6] != 0 )
	{
		Touch_X = ((aa1 * Touch_AD_X) + (bb1 * Touch_AD_Y) + cc1 );
		Touch_Y = ((aa2 * Touch_AD_X) + (bb2 * Touch_AD_Y) + cc2 );
		return 0;
	}
	else
	{
		return 1;
	}
}



u8 Touch_Calibrate()
{
	u8 i;
	u16 test_x=0, test_y=0;
	u16 gap_x=0, gap_y=0;
	u16 CalibrateSample[4][2];
	// delay_init();
	NVIC->ICER[EXTI9_5_IRQn >> 0x05] = (uint32_t)0x01 << (EXTI9_5_IRQn & (uint8_t)0x1F);//失能中断
	LCD_SetDirection(0);//0号手持方向
	LCD_SetWindow(0,0,240,320);
	TextColor=Black;
	for(i=0;i<4;i++)
	{
		LCD_Clear(White);
		LCD_DisplayString(Line0,"Touch Calibrate");//显示提示信息
		Touch_DrawCross(CalibrateCoord[i][0],CalibrateCoord[i][1]);//显示校正用的“十”字
		while(Touch_GetXY_Real()!=0);
		delay_ms(4000);
		CalibrateSample[i][0] = Touch_AD_X;
		CalibrateSample[i][1] = Touch_AD_Y;
	}

	/* 用原始参数计算出 原始参数与坐标的转换系数。 */
	Touch_CalcPara(CalibrateCoord,CalibrateSample);
	/*计算X值*/
	test_x = ( (touch_para[0] * CalibrateSample[3][0]) + (touch_para[1] * CalibrateSample[3][1]) + touch_para[2] ) / touch_para[6] ;
	/*计算Y值*/
	test_y = ( (touch_para[3] * CalibrateSample[3][0]) + (touch_para[4] * CalibrateSample[3][1]) + touch_para[5] ) / touch_para[6] ;

	gap_x = (test_x > CalibrateCoord[3][0])?(test_x - CalibrateCoord[3][0]):(CalibrateCoord[3][0] - test_x);
	gap_x = (test_y > CalibrateCoord[3][1])?(test_y - CalibrateCoord[3][1]):(CalibrateCoord[3][1] - test_y);


	LCD_Clear(White);
	if((gap_x>11) || (gap_y>11))
	{
		LCD_DisplayString(Line4,"Calibrate fail");
		delay_ms(2000);
		Touch_Calibrate();
		return 1;
	}
	aa1 = (touch_para[0]*1.0)/touch_para[6];
	bb1 = (touch_para[1]*1.0)/touch_para[6];
	cc1 = (touch_para[2]*1.0)/touch_para[6];

	aa2 = (touch_para[3]*1.0)/touch_para[6];
	bb2 = (touch_para[4]*1.0)/touch_para[6];
	cc2 = (touch_para[5]*1.0)/touch_para[6];

	LCD_DisplayString(Line4,"Calibrate Success");
	delay_ms(2000);
	NVIC->ISER[EXTI9_5_IRQn >> 0x05] =(uint32_t)0x01 << (EXTI9_5_IRQn & (uint8_t)0x1F);//使能中断
	return 0;
}

void Touch_DrawCross(u16 x, u16 y)
{
	u8 i,j;

	x = x-7;
	y = y-1;
	for(j=0; j<3; j++)
	{
		LCD_SetCursor(x,y);
		LCD_WriteRAM_Prepare();
		for(i=0; i<15; i++)
		{
			LCD_WriteRAM(TextColor);
		}
		y++;
	}

	x = x+6;
	y = y-6;
	for(j=0; j<15; j++)
	{
		LCD_SetCursor(x,y);
		LCD_WriteRAM_Prepare();
		for(i=0; i<3; i++)
		{
			LCD_WriteRAM(TextColor);
		}
		y++;
	}
}

void EXTI9_5_IRQHandler(void)
{
	if(EXTI_GetITStatus(EXTI_Line8) != RESET)
	{
		Touch_GetPoint();
		LCD_DrawBigPoint(Touch_X,Touch_Y);
		EXTI_ClearITPendingBit(EXTI_Line8);
	}
}
