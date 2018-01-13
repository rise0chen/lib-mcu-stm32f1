#include "SCCB.hpp"

#define DEV_ADR  ADDR_OV7725 			 /*设备地址定义*/

/********************************************************************
 * 函数名：SCCB_Configuration
 * 描述  ：SCCB管脚配置
 * 输入  ：无
 * 输出  ：无
 * 注意  ：无
 ********************************************************************/
void SCCB_GPIO_Configuration(void)
{
  GPIO_InitTypeDef  GPIO_InitStructure;
  /* SCL(PC6)、SDA(PC7)管脚配置 */
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
  GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_6 | GPIO_Pin_7;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD;
  GPIO_Init(GPIOC, &GPIO_InitStructure);
}



/********************************************************************
 * 函数名：SCCB_delay
 * 描述  ：延迟时间
 * 输入  ：无
 * 输出  ：无
 * 注意  ：内部调用
 ********************************************************************/
static void SCCB_delay(void)
{
   u16 i = 400;
   while(i)
   {
     i--;
   }
}



/********************************************************************
 * 函数名：SCCB_Start
 * 描述  ：SCCB起始信号
 * 输入  ：无
 * 输出  ：无
 * 注意  ：内部调用
 ********************************************************************/
static int SCCB_Start(void)
{
	SDA_H;
	SCL_H;
	SCCB_delay();
	if(!SDA_read)
	return DISABLE;	/* SDA线为低电平则总线忙,退出 */
	SDA_L;
	SCCB_delay();
	if(SDA_read)
	return DISABLE;	/* SDA线为高电平则总线出错,退出 */
	SDA_L;
	SCCB_delay();
	return ENABLE;
}



/********************************************************************
 * 函数名：SCCB_Stop
 * 描述  ：SCCB停止信号
 * 输入  ：无
 * 输出  ：无
 * 注意  ：内部调用
 ********************************************************************/
static void SCCB_Stop(void)
{
	SCL_L;
	SCCB_delay();
	SDA_L;
	SCCB_delay();
	SCL_H;		   /* SCL线为高电平时，SDA线状态变换表示停止信号 */
	SCCB_delay();
	SDA_H;
	SCCB_delay();
}







/********************************************************************
 * 函数名：SCCB_NoAck
 * 描述  ：SCCB 无应答方式
 * 输入  ：无
 * 输出  ：无
 * 注意  ：内部调用
 ********************************************************************/
static void SCCB_NoAck(void)
{
	SCL_L;
	SCCB_delay();
	SDA_H;	//高电平为非应答信号
	SCCB_delay();
	SCL_H;
	SCCB_delay();
	SCL_L;
	SCCB_delay();
}




/********************************************************************
 * 函数名：SCCB_WaitAck
 * 描述  ：SCCB 等待应答
 * 输入  ：无
 * 输出  ：返回为:=1有ACK,=0无ACK
 * 注意  ：内部调用
 ********************************************************************/
static int SCCB_WaitAck(void) 
{
	SCL_L;
	SCCB_delay();
	SDA_H;
	SCCB_delay();
	SCL_H;
	SCCB_delay();
	if(SDA_read)
	{
      SCL_L;
      return DISABLE;
	}
	SCL_L;		//SDA低电平应答
	return ENABLE;
}



 /*******************************************************************
 * 函数名：SCCB_SendByte
 * 描述  ：数据从高位到低位
 * 输入  ：SendByte: 发送的数据
 * 输出  ：无
 * 注意  ：内部调用
 *********************************************************************/
static void SCCB_SendByte(u8 SendByte)
{
    u8 i=8;
    while(i--)
    {
        SCL_L;
        SCCB_delay();
      if(SendByte&0x80)
        SDA_H;
      else
        SDA_L;
        SendByte<<=1;
        SCCB_delay();
		SCL_H;
        SCCB_delay();
    }
    SCL_L;
}




 /******************************************************************
 * 函数名：SCCB_ReceiveByte
 * 描述  ：数据从高位到低位
 * 输入  ：无
 * 输出  ：SCCB总线返回的数据
 * 注意  ：内部调用
 *******************************************************************/
static int SCCB_ReceiveByte(void)
{
    u8 i=8;
    u8 ReceiveByte=0;

    SDA_H;
    while(i--)
    {
      ReceiveByte<<=1;
      SCL_L;
      SCCB_delay();
	  SCL_H;
      SCCB_delay();
      if(SDA_read)
      {
        ReceiveByte|=0x01;
      }
    }
    SCL_L;
    return ReceiveByte;
}





 /*****************************************************************************************
 * 函数名：SCCB_WriteByte
 * 描述  ：写一字节数据
 * 输入  ：- WriteAddress: 待写入地址 	- SendByte: 待写入数据	- DeviceAddress: 器件类型
 * 输出  ：返回为:=1成功写入,=0失败
 * 注意  ：无
 *****************************************************************************************/
int SCCB_WriteByte( u16 WriteAddress , u8 SendByte )
{
    if(!SCCB_Start())
	{
	    return DISABLE;
	}
    SCCB_SendByte( DEV_ADR );                    /* 器件地址，第一阶段*/
    if( !SCCB_WaitAck() )
	{
		SCCB_Stop();
		return DISABLE;
	}
    SCCB_SendByte((u8)(WriteAddress & 0x00FF));   /* 设置低起始地址，第二阶段 */
    SCCB_WaitAck();
    SCCB_SendByte(SendByte);					  /* 写内容，第三阶段 */
    SCCB_WaitAck();
    SCCB_Stop();
    return ENABLE;
}




/******************************************************************************************************************
 * 函数名：SCCB_ReadByte
 * 描述  ：读取一串数据
 * 输入  ：- pBuffer: 存放读出数据 		- ReadAddress: 待读出地址		 - DeviceAddress: 器件类型
 * 输出  ：返回为:=1成功读入,=0失败
 * 注意  ：无
 **********************************************************************************************************************/
int SCCB_ReadByte(u8* pBuffer,  u8 ReadAddress)
{

    if(!SCCB_Start())
	{
	    return DISABLE;
	}								  /*在读操作前要有写操作以提供起始地址*/
    SCCB_SendByte( DEV_ADR );         /* 器件地址，写操作第一阶段 */
    if( !SCCB_WaitAck() )
	{
		SCCB_Stop();
		return DISABLE;
	}
    SCCB_SendByte( ReadAddress );    /* 设置低起始地址，写操作第二阶段 */
    SCCB_WaitAck();
    SCCB_Stop();

    if(!SCCB_Start())
	{
		return DISABLE;
	}
    SCCB_SendByte( DEV_ADR + 1 );               /* 器件地址，读操作第一阶段 */
    if(!SCCB_WaitAck())
	{
		SCCB_Stop();
		return DISABLE;
	}

    *pBuffer = SCCB_ReceiveByte();			    /* 读取内容，读操作第二阶段 */
	SCCB_NoAck();

    SCCB_Stop();
    return ENABLE;
}




