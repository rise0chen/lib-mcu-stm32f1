#include "OV7670.hpp"
#include "SCCB.hpp"

#define Delay_ms(time)  Delay_us(time*100)
#define OV7670_REG_NUM 114
u8 volatile OV7670_VSYNC;	 /* 帧同步信号 */
volatile u8 Frame_Count = 0 ;

u8 Sensor_Config[][2] =
{
/*以下为OV7670 QVGA RGB565参数 {地址, 值} */
{0x3a, 0x04},{0x40, 0x10},{0x12, 0x14},{0x32, 0x80},{0x17, 0x16},

{0x18, 0x04},{0x19, 0x02},{0x1a, 0x7b},{0x03, 0x06},{0x0c, 0x0c},{0x15, 0x02},
{0x3e, 0x00},{0x70, 0x00},{0x71, 0x01},{0x72, 0x11},{0x73, 0x09},

{0xa2, 0x02},{0x11, 0x00},{0x7a, 0x20},{0x7b, 0x1c},{0x7c, 0x28},

{0x7d, 0x3c},{0x7e, 0x55},{0x7f, 0x68},{0x80, 0x76},{0x81, 0x80},

{0x82, 0x88},{0x83, 0x8f},{0x84, 0x96},{0x85, 0xa3},{0x86, 0xaf},

{0x87, 0xc4},{0x88, 0xd7},{0x89, 0xe8},{0x13, 0xe0},{0x00, 0x00},

{0x10, 0x00},{0x0d, 0x00},{0x14, 0x30},{0xa5, 0x05},{0xab, 0x07},

{0x24, 0x75},{0x25, 0x63},{0x26, 0xA5},{0x9f, 0x78},{0xa0, 0x68},

{0xa1, 0x03},{0xa6, 0xdf},{0xa7, 0xdf},{0xa8, 0xf0},{0xa9, 0x90},

{0xaa, 0x94},{0x13, 0xf5},{0x0e, 0x61},{0x0f, 0x4b},{0x16, 0x02},

{0x1e, 0x07},{0x21, 0x02},{0x22, 0x91},{0x29, 0x07},{0x33, 0x0b},

{0x35, 0x0b},{0x37, 0x1d},{0x38, 0x71},{0x39, 0x2a},{0x3c, 0x78},

{0x4d, 0x40},{0x4e, 0x20},{0x69, 0x5d},{0x6b, 0x40},{0x74, 0x19},{0x8d, 0x4f},

{0x8e, 0x00},{0x8f, 0x00},{0x90, 0x00},{0x91, 0x00},{0x92, 0x00},

{0x96, 0x00},{0x9a, 0x80},{0xb0, 0x84},{0xb1, 0x0c},{0xb2, 0x0e},

{0xb3, 0x82},{0xb8, 0x0a},{0x43, 0x14},{0x44, 0xf0},{0x45, 0x34},

{0x46, 0x58},{0x47, 0x28},{0x48, 0x3a},{0x59, 0x88},{0x5a, 0x88},

{0x5b, 0x44},{0x5c, 0x67},{0x5d, 0x49},{0x5e, 0x0e},{0x64, 0x04},{0x65, 0x20},

{0x66, 0x05},{0x94, 0x04},{0x95, 0x08},{0x6c, 0x0a},{0x6d, 0x55},


{0x4f, 0x80},{0x50, 0x80},{0x51, 0x00},{0x52, 0x22},{0x53, 0x5e},{0x54, 0x80},

{0x6e, 0x11},{0x6f, 0x9f},{0x55, 0x00},{0x56, 0x45},{0x57, 0x80},
};


/************************************************
 * 函数名：FIFO_GPIO_Configuration
 * 描述  ：FIFO GPIO配置
 * 输入  ：无
 * 输出  ：无
 * 注意  ：无
 ************************************************/
void FIFO_GPIO_Configuration(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOB | RCC_APB2Periph_GPIOC | RCC_APB2Periph_GPIOD | RCC_APB2Periph_GPIOE, ENABLE);

    /*PB5(FIFO_WRST--FIFO写复位)*/
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    /*PD3(FIFO_WEN--FIFO写使能)  PD6(FIFO_REN--FIFO读使能)*/
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3 | GPIO_Pin_6;
    GPIO_Init(GPIOD, &GPIO_InitStructure);

    /*PE0(FIFO_RRST--FIFO读复位)  PE2(FIFO_RCLK-FIFO读时钟)*/
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_2;
    GPIO_Init(GPIOE, &GPIO_InitStructure);

    /*PB8-PB15(FIFO_DATA--FIFO输出数据)*/
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_10 | GPIO_Pin_11 | GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    FIFO_CS_L();	  					/*拉低使FIFO输出使能*/
    FIFO_WE_H();   						/*拉高使FIFO写允许*/

}




/************************************************
 * 函数名：VSYNC_GPIO_Configuration
 * 描述  ：OV7670 GPIO配置
 * 输入  ：无
 * 输出  ：无
 * 注意  ：无
 ************************************************/
static void VSYNC_GPIO_Configuration(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);	  /*PA0---VSYNC*/
    GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_0;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
}



/************************************************
 * 函数名：VSYNC_NVIC_Configuration
 * 描述  ：VSYNC中断配置
 * 输入  ：无
 * 输出  ：无
 * 注意  ：无
 ************************************************/
static  void VSYNC_NVIC_Configuration(void)
{
    NVIC_InitTypeDef NVIC_InitStructure;
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);
    NVIC_InitStructure.NVIC_IRQChannel = EXTI0_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
}





/************************************************
 * 函数名：VSYNC_EXTI_Configuration
 * 描述  ：OV7670 VSYNC中断管脚配置
 * 输入  ：无
 * 输出  ：无
 * 注意  ：无
 ************************************************/
static  void VSYNC_EXTI_Configuration(void)
{
    EXTI_InitTypeDef EXTI_InitStructure;

    GPIO_EXTILineConfig(GPIO_PortSourceGPIOA, GPIO_PinSource0);
    EXTI_InitStructure.EXTI_Line = EXTI_Line0;
    EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
    EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising ; /*上升沿触发*/
    EXTI_InitStructure.EXTI_LineCmd = ENABLE;
    EXTI_Init(&EXTI_InitStructure);
    EXTI_GenerateSWInterrupt(EXTI_Line0);	/*中断挂到 EXTI_Line0  线*/
}





/************************************************
 * 函数名：VSYNC_Init
 * 描述  ：OV7670 VSYNC中断相关配置
 * 输入  ：无
 * 输出  ：无
 * 注意  ：无
 ************************************************/
void VSYNC_Init(void)
{
    VSYNC_GPIO_Configuration();
    VSYNC_EXTI_Configuration();
    VSYNC_NVIC_Configuration();
}







/************************************************
 * 函数名：Sensor_Init
 * 描述  ：Sensor初始化
 * 输入  ：无
 * 输出  ：返回1成功，返回0失败
 * 注意  ：无
 ************************************************/
ErrorStatus OV7670_Init(void)
{
	u16 i = 0;
	u8 Sensor_IDCode = 0;
	if( 0 == SCCB_WriteByte ( 0x12, 0x80 ) ) /*复位sensor */
		return ERROR ;
	if( 0 == SCCB_ReadByte( &Sensor_IDCode,  0x0b ) )	 /* 读取sensor ID号*/
		return ERROR;

	if(Sensor_IDCode == OV7670_ID)
	{
		for( i = 0 ; i < OV7670_REG_NUM ; i++ )
		{
			if( 0 == SCCB_WriteByte(Sensor_Config[i][0], Sensor_Config[i][1]) )
				return ERROR;
		}
	}
	else
	{
		return ERROR;
	}
	return SUCCESS;
}


