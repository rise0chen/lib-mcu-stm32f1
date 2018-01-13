#include "LCD12864.hpp"

namespace LCD12864{
	u8 STA;
	u32  fontaddr=0;
	gpio SDI =gpio(PC, 8);
	gpio SCK =gpio(PC, 6);
	gpio A0  =gpio(PC, 7);
	gpio RST=gpio(PA,8);
	gpio CS  =gpio(PC,9);
	gpio BLEN=gpio(PA,11);
	
	void Init(void){
		SDI.Config(P_PPO);
		SCK.Config(P_PPO);
		A0.Config(P_PPO);
		RST.Config(P_PPO);
		CS.Config(P_PPO);
		BLEN.Config(P_PPO);
		
		Close();
	}
	void WriteCmd(u8 data){//写指令
		u8 i;
		*CS.O=0;
		*A0.O=0;
		for(i=0;i<8;i++){
			*SCK.O=0;
			if(data&0x80) *SDI.O=1;
			else *SDI.O=0;
			*SCK.O=1;
			data<<=1;
		}
		*CS.O=1;
	}
	void WriteData(u8 data){//写数据
		u8 i;
		*CS.O=0;
		*A0.O=1;
		for(i=0;i<8;i++){
			*SCK.O=0;
			if(data&0x80) *SDI.O=1;
			else *SDI.O=0;
			*SCK.O=1;
			data<<=1;
		}
		*CS.O=1;
	}
	void Reset(void){
		*RST.O=0;//低电平复位
		delay_ms(50);
		*RST.O=1;//复位完毕
		delay_ms(50);
		WriteCmd(0xe2);//软复位
		delay_ms(20);
		
		WriteCmd(0x60);//Set Display Start Line
		WriteCmd(0xa2);//Bais set
		WriteCmd(0xa0);//ADC seg镜像选择 0xa0正常,0xa1左右镜像
		WriteCmd(0xc0);//com镜像选择	   0xc0正常,0xc8上下镜像
		WriteCmd(0x2c);//内部电源管理
		WriteCmd(0x2e);
		WriteCmd(0x2f);
		WriteCmd(0x81);
		WriteCmd(0x16);//电压调整寄存器低位	范围:0x00~0x3f
		WriteCmd(0x25);//电压调整寄存器高位	范围:0x21~0x27	值越大，显示效果越浓
		WriteCmd(0x40);//从首行开始显示
		Clear();
	}
	void Open(void){
		STA=1;
		WriteCmd(0xaf);//显示开
		*BLEN.O=1;//背光开
	}
	void Close(void){
		Reset();
		
		STA=0;
		WriteCmd(0xae);//显示关
		*BLEN.O=0;//背光关
	}
	void Clear(void){//全屏清屏
		u8 i,j;
		for(i=0;i<8;i++){
			SetAddr(1+i,1);
			for(j=1;j<132;j++){
				WriteData(0x00);
			}
		}
		//SetAddr(1,1);
		delay_ms(1);
	}
	void SetAddr(u8 page,u8 column){//设置液晶屏的显示地址
		column=column-1;
		page=page-1;
		WriteCmd(0xb0+page);//设置页地址。每页是8行。一个画面的64行被分成8个页。我们平常所说的第1页，在LCD驱动IC里是第0页，所以在这里减去1
		WriteCmd(((column>>4)&0x0f)+0x10);//设置列地址的高4位
		WriteCmd(column&0x0f);//设置列地址的低4位
	}
	
	
	void DisplayFull(u8 data1,u8 data2){//出厂电测用的画面，比如全显，隔行显示，隔列显示等等
		u8 i,j;
		for(i=0;i<8;i++){
			SetAddr(1+i,1);
			for(j=0;j<66;j++){
				WriteData(data1);
				WriteData(data2);
			}
		}
	}
	//显示16x16点阵的汉字/全角符号/全角标点，或8x16点阵的数字/英文/半角标点/ASCII码符号，
	//当“reverse=1”时选择反显，当“reverse=0”时选择正显
	void DisplayGBKString(u8 page,u8 column,u8 reverse,char* text){
		u8 i= 0;
		while((text[i]>0x00)&&i<16){
			if(((text[i]>=0xb0) &&(text[i]<=0xf7))&&(text[i+1]>=0xa1)){
				//国标简体（GB2312）汉字在晶联讯字库IC中的地址由以下公式来计算：
				//Address = ((MSB - 0xB0) * 94 + (LSB - 0xA1)+ 846)*32+ BaseAdd;BaseAdd=0
				//由于担心8位单片机有乘法溢出问题，所以分三部取地址
				fontaddr = (text[i]- 0xb0)*94; 
				fontaddr += (text[i+1]-0xa1)+846;
				fontaddr = (u32)(fontaddr*32);
				FontRom::ReadWrite16x16(fontaddr,reverse,page,column);//从指定地址读出数据写到液晶屏指定（page,column)座标中
				i+=2;
				column+=16;
			}else if(((text[i]>=0xa1) &&(text[i]<=0xa9))&&(text[i+1]>=0xa1)){
				//国标简体（GB2312）15x16点的字符在晶联讯字库IC中的地址由以下公式来计算：
				//Address = ((MSB - 0xa1) * 94 + (LSB - 0xA1))*32+ BaseAdd;BaseAdd=0
				//由于担心8位单片机有乘法溢出问题，所以分三部取地址
				fontaddr = (text[i]- 0xa1)*94; 
				fontaddr += (text[i+1]-0xa1);
				fontaddr = (u32)(fontaddr*32);
				FontRom::ReadWrite16x16(fontaddr,reverse,page,column);//从指定地址读出数据写到液晶屏指定（page,column)座标中
				i+=2;
				column+=16;
			}else if((text[i]>=0xa4) && (text[i]<=0xa8) &&(text[i+1]>=0xa1)){
				fontaddr = 0;
				FontRom::ReadWrite16x16(fontaddr,reverse,page,column);//从指定地址读出数据写到液晶屏指定（page,column)座标中
				i+=2;
				column+=16;
			}else if((text[i]>=0x20) &&(text[i]<=0x7e)){
				fontaddr = (text[i]- 0x20);
				fontaddr = (u32)(fontaddr*16);
				fontaddr = (u32)(fontaddr+0x3b7c0);
				FontRom::ReadWrite8x16(fontaddr,reverse,page,column);//从指定地址读出数据写到液晶屏指定（page,column)座标中
				i+=1;
				column+=8;
			}else i++;
		}
		delay_ms(1);
	}
	//显示5x8点阵数字/半角符号/半角标点
	//当“reverse=1”时选择反显，当“reverse=0”时选择正显
	void Display5x8String(u8 page,u8 column,u8 reverse,char* text){
		u8 i= 0;
		while((text[i]>0x00)){
			if((text[i]>=0x20) &&(text[i]<=0x7e)){
				fontaddr = (text[i]- 0x20);
				fontaddr = (u32)(fontaddr*8);
				fontaddr = (u32)(fontaddr+0x3bfc0);
				FontRom::ReadWrite5x8(fontaddr,reverse,page,column);//从指定地址读出数据写到液晶屏指定（page,column)座标中
				if(reverse==1){WriteData(0xff);}else{WriteData(0x00);}
				i+=1;
				column+=6;
			}else{i++;}
		}
	}
	//显示单个的8x16的数字、英文或其他ASCII范围内的字符到LCD上，括号内的参数分别为（页地址，列地址，ASCII代码）
	void Display8x16Char(u8 page,u8 column,u8 ASCII_CODE){
		u8 j,k,n,m,data;
		j=ASCII_CODE-0x20;//ASCII的机器码是从0x20开始的，对应我们的数组就是从0开始，所以减去0x20
		for(n=0;n<2;n++){
			if(n==1){m=0;}
			else if(n==0){m=1;}
			SetAddr(page+m,column);//指定页地址，列地址
			for(k=0;k<8;k++){
				data = FontRom::convert(AsciiTable8x16[j][k+n*8]);
				WriteData(data);
				//WriteData(ascii_table_8x16[j][k+n*8]);//传输数据:这个数据来自于"ascii_table_8x16[95][16]"这个二维数组
			}
		}
	}
	
}
