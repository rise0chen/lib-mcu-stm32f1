#include "sdcard.hpp"
#include "spi.hpp"

u8  SD_Type=0;//SD卡的类型

u8 SD_disk_initialize(void)
{
	u8 r1;				// 存放SD卡的返回值
	u16 retry=20;	// 用来进行超时计数
	u8 buff[4];
	u16 i;

	SPI_Config(SPI1, 0);
	SPI_Speed(SPI1,0);
	for(i=0;i<10;i++){SPI_SendByte(0xFF);}//等待大于74个脉冲

	do
	{
		r1=SD_SendCmd(CMD0,0,0x95);//进入IDLE状态
		printf("%x\n",r1);
	}while((r1!=0X01) && retry--);
	//while((SD_SendCmd(CMD0,0,0x95)!=0X01) && --retry);

	SD_Type=0;//默认无卡
	if(r1==0X01)
	{
		if(SD_SendCmd(CMD8,0x1AA,0x87)==1)//SD V2.0
		{
			for(i=0;i<4;i++)buff[i]=SPI_SendByte(0XFF);	//Get trailing return value of R7 resp
			if(buff[2]==0X01&&buff[3]==0XAA)//卡是否支持2.7~3.6V
			{
				retry=0XFFFE;
				do
				{
					SD_SendCmd(CMD55,0,0X01);	//发送CMD55
					r1=SD_SendCmd(CMD41,0x40000000,0X01);//发送CMD41
				}while(r1&&retry--);
				if(retry&&SD_SendCmd(CMD58,0,0X01)==0)//鉴别SD2.0卡版本开始
				{
					for(i=0;i<4;i++)buff[i]=SPI_SendByte(0XFF);//得到OCR值
					if(buff[0]&0x40)SD_Type=SD_TYPE_V2HC;//检查CCS
					else SD_Type=SD_TYPE_V2;
				}
			}
		}else//SD V1.x/ MMC	V3
		{
			SD_SendCmd(CMD55,0,0X01);		//发送CMD55
			r1=SD_SendCmd(CMD41,0,0X01);	//发送CMD41
			if(r1<=1)
			{
				SD_Type=SD_TYPE_V1;
				retry=0XFE;
				do//等待退出IDLE模式
				{
					SD_SendCmd(CMD55,0,0X01);	//发送CMD55
					r1=SD_SendCmd(CMD41,0,0X01);//发送CMD41
				}while(r1&&retry--);
			}else//MMC卡不支持CMD55+CMD41识别
			{
				SD_Type=SD_TYPE_MMC;//MMC V3
				retry=0XFFFE;
				do//等待退出IDLE模式
				{
					r1=SD_SendCmd(CMD1,0,0X01);//发送CMD1
				}while(r1&&retry--);
			}
			if(retry==0||SD_SendCmd(CMD16,512,0X01)!=0)SD_Type=SD_TYPE_ERR;//错误的卡
		}
	}
	SD_DisSelect();//取消片选
	SPI_Speed(SPI1,6);//高速
	if(SD_Type)return 0;
	else if(r1)return r1; 
	return 0xaa;//其他错误
}
//读SD卡
u8 SD_disk_read(u8 *buff,u32 sector,u8 count)
{
	u8 r1;
	if(SD_Type!=SD_TYPE_V2HC)sector <<= 9;//转换为字节地址
	if(count==1)
	{
		r1=SD_SendCmd(CMD17,sector,0X01);//读命令
		if(r1==0)//指令发送成功
		{
			r1=SD_RecvData(buff,512);//接收512个字节
		}
	}else
	{
		r1=SD_SendCmd(CMD18,sector,0X01);//连续读命令
		do
		{
			r1=SD_RecvData(buff,512);//接收512个字节
			buff+=512;
		}while(--count && r1==0); 
		SD_SendCmd(CMD12,0,0X01);	//发送停止命令
	}
	SD_DisSelect();//取消片选
	return r1;
}
//写SD卡
u8 SD_disk_write(u8 *buff,u32 sector,u8 count)
{
	u8 r1;
	if(SD_Type!=SD_TYPE_V2HC)sector *= 512;//转换为字节地址
	if(count==1)
	{
		r1=SD_SendCmd(CMD24,sector,0X01);//读命令
		if(r1==0)//指令发送成功
		{
			r1=SD_SendBlock(buff,0xFE);//写512个字节
		}
	}else
	{
		if(SD_Type!=SD_TYPE_MMC)
		{
			SD_SendCmd(CMD55,0,0X01);
			SD_SendCmd(CMD23,count,0X01);//发送指令
		}
		r1=SD_SendCmd(CMD25,sector,0X01);//连续读命令
		if(r1==0)
		{
			do
			{
				r1=SD_SendBlock(buff,0xFC);//接收512个字节
				buff+=512;
			}while(--count && r1==0);
			r1=SD_SendBlock(0,0xFD);//接收512个字节
		}
	}
	SD_DisSelect();//取消片选
	return r1;
}

//等待卡准备好
u8 SD_WaitReady(void)
{
	u32 t=0;
	do
	{
		if(SPI_SendByte(0XFF)==0XFF) return 0;//OK
		t++;
	}while(t<0XFFFF);//等待
	return 1;
}


//获取SD卡的总扇区数（扇区数）
//返回值:0： 取容量出错
//       其他:SD卡的容量(扇区数/512字节)
//每扇区的字节数必为512，因为如果不是512，则初始化不能通过.
u32 SD_GetSectorCount(void)
{
	u8 csd[16];
	u32 Capacity;
	u8 n;
	u16 csize;
	//取CSD信息，如果期间出错，返回0
	if(SD_GetCSD(csd)!=0) return 0;
	//如果为SDHC卡，按照下面方式计算
	if((csd[0]&0xC0)==0x40)	//V2.00的卡
	{
		csize = csd[9] + ((u16)csd[8] << 8) + 1;
		Capacity = (u32)csize << 10;//得到扇区数
	}else//V1.XX的卡
	{
		n = (csd[5] & 15) + ((csd[10] & 128) >> 7) + ((csd[9] & 3) << 1) + 2;
		csize = (csd[8] >> 6) + ((u16)csd[7] << 2) + ((u16)(csd[6] & 3) << 10) + 1;
		Capacity= (u32)csize << (n - 9);//得到扇区数
	}
	return Capacity;
}

//取消选择,释放SPI总线
void SD_DisSelect(void)
{
	SPI_CS(1);
	SPI_SendByte(0xff);//提供额外的8个时钟
}
//选择sd卡,并且等待卡准备OK
u8 SD_Select(void)
{
	SPI_CS(0);
	if(SD_WaitReady()==0) return 0;//等待成功
	SD_DisSelect();
	return 1;//等待失败
}
//等待SD卡回应
u8 SD_GetResponse(u8 Response)
{
	u16 Count=0xFFFF;//等待次数	   
	while ((SPI_SendByte(0XFF)!=Response)&&Count)Count--;//等待得到准确的回应
	if (Count==0)return MSD_RESPONSE_FAILURE;//得到回应失败
	else return MSD_RESPONSE_NO_ERROR;//正确回应
}
//从sd卡读取一个数据包的内容
u8 SD_RecvData(u8*buff,u16 len)
{
	if(SD_GetResponse(0xFE)) return 1;//等待SD卡发回数据起始令牌0xFE
	while(len--)//开始接收数据
	{
		*buff=SPI_SendByte(0xFF);
		buff++;
	}
	//下面是2个伪CRC（dummy CRC）
	SPI_SendByte(0xFF);
	SPI_SendByte(0xFF);
	return 0;//读取成功
}
//向sd卡写入一个数据包的内容 512字节
u8 SD_SendBlock(u8*buff,u8 cmd)
{
	u16 t;
	if(SD_WaitReady())return 1;//等待准备失效
	SPI_SendByte(cmd);
	if(cmd!=0XFD)//不是结束指令
	{
		for(t=0;t<512;t++)SPI_SendByte(buff[t]);
	    SPI_SendByte(0xFF);//忽略crc
	    SPI_SendByte(0xFF);
		t=SPI_SendByte(0xFF);//接收响应
		if((t&0x1F)!=0x05) return 2;//响应错误
	}
	return 0;//写入成功
}
//向SD卡发送一个命令
u8 SD_SendCmd(u8 cmd, u32 arg, u8 crc)
{
	u8 r1;
	u8 Retry=0;
	SD_DisSelect();//取消上次片选
	if(SD_Select())return 0XFF;//片选失效
	//发送
	SPI_SendByte(cmd | 0x40);//分别写入命令
	SPI_SendByte(arg >> 24);
	SPI_SendByte(arg >> 16);
	SPI_SendByte(arg >> 8);
	SPI_SendByte(arg);
	SPI_SendByte(crc);
	if(cmd==CMD12)SPI_SendByte(0xff);//Skip a stuff byte when stop reading
	//等待响应，或超时退出
	Retry=0X1F;
	do
	{
		r1=SPI_SendByte(0xFF);
	}while((r1&0X80) && Retry--);
	//返回状态值
	return r1;
}
//获取SD卡的CID信息，包括制造商信息
//输入: u8 *cid_data(存放CID的内存，至少16Byte）
u8 SD_GetCID(u8 *cid_data)
{
	u8 r1;
	//发CMD10命令，读CID
	r1=SD_SendCmd(CMD10,0,0x01);
	if(r1==0x00)
	{
		r1=SD_RecvData(cid_data,16);//接收16个字节的数据
	}
	SD_DisSelect();//取消片选
	if(r1)return 1;
	else return 0;
}
//获取SD卡的CSD信息，包括容量和速度信息
//输入:u8 *cid_data(存放CID的内存，至少16Byte）
u8 SD_GetCSD(u8 *csd_data)
{
	u8 r1;
	r1=SD_SendCmd(CMD9,0,0x01);//发CMD9命令，读CSD
	if(r1==0)
	{
		r1=SD_RecvData(csd_data, 16);//接收16个字节的数据
	}
	SD_DisSelect();//取消片选
	if(r1)return 1;
	else return 0;
}
