/*************************************************
Copyright (C), 2018-2028, Crise Tech. Co., Ltd.
File name: RFID.cpp
Author: rise0chen
Version: 1.0
Date: 2018.4.26
Description: RFID 类
Usage:
	#include "RFID.hpp"
	RFID rfid(&spi2, &gpio_default, &gpio_default);
	rfid.init();
History: 
	rise0chen   2018.4.26   编写注释
*************************************************/
#include "RFID.hpp"

/*************************************************
Function: RFID::RFID
Description: RFID构造函数
Input: 
	spi_com SPI通信外设
	spi_CS  SPI片选引脚
	pin_RST SPI复位引脚
Return: RFID类
*************************************************/
RFID::RFID(Spi *spi_com, Gpio *spi_CS, Gpio *pin_RST){
	com = spi_com;
	CS  = spi_CS;
	RST = pin_RST;
}

/*************************************************
Function: RFID::init
Description: 初始化RC522芯片
Input: void
Return: void
*************************************************/
void RFID::init(void){
	RST->config(P_PPO);
	CS->config(P_PPO);
	PcdReset();
	closeAntenna();
	openAntenna();
}

/*************************************************
Function: RFID::writeRawRC
Description: 写RC522寄存器
Input: 
	addr  寄存器地址
	value 写入数值
Return: void
*************************************************/
void RFID::writeRawRC(u8 addr, u8 value){
	*CS->O=0;//选中
	//寄存器地址格式:0XXXXXX0
	com->rwByte((addr << 1) &0x7E);
	com->rwByte(value);
	*CS->O=1;//放开
}

/*************************************************
Function: RFID::readRawRC
Description: 读RC522寄存器
Input: 
	addr  寄存器地址
Return: 读出的值
*************************************************/
u8 RFID::readRawRC(u8 addr){
	u8 data;

	*CS->O=0;//选中
	//寄存器地址格式:1XXXXXX0
	com->rwByte((addr << 1) | 0x80);
	data = com->rwByte(0x00);
	*CS->O=1;//放开
	return(data);
}

/*************************************************
Function: RFID::setBitMask
Description: 置位
Input: 
	reg   寄存器地址
	mask  置位值
Return: void
*************************************************/
void RFID::setBitMask(u8 reg, u8 mask){
	u8 tmp = 0x0;
	tmp = readRawRC(reg);
	writeRawRC(reg, tmp | mask);// set bit mask
}

/*************************************************
Function: RFID::clearBitMask
Description: 复位
Input: 
	reg   寄存器地址
	mask  置位值
Return: void
*************************************************/
void RFID::clearBitMask(u8 reg, u8 mask){
	u8 tmp = 0x00;
	tmp = readRawRC(reg);
	writeRawRC(reg, tmp &~mask);// clear bit mask
}

/*************************************************
Function: RFID::closeAntenna
Description: 关闭天线
Input: void
Return: void
*************************************************/
void RFID::closeAntenna(void){
	clearBitMask(TxControlReg, 0x03);//禁止Tx1RFEn,Tx2RFEn
}

/*************************************************
Function: RFID::openAntenna
Description: 开启天线(每次启动或关闭天险发射之间应至少有1ms的间隔)
Input: void
Return: void
*************************************************/
void RFID::openAntenna(void){
	u8 i;
	i = readRawRC(TxControlReg);//读取出发送控制寄存器
	if(!(i &0x03)){//如果未开启,则
		setBitMask(TxControlReg, 0x03);//开启Tx1RFEn,Tx2RFEn
	}
}

/*************************************************
Function: RFID::PcdComMF522
Description: 通过RC522和ISO14443卡通讯
Input: 
	Command    RC522命令字
	pDataIn    通过RC522发送到卡片的数据
	InLenByte  发送数据的字节长度
	pDataOut   接收到的卡片返回数据
	pOutLenBit 返回数据的位长度
Return: 错误码 0成功
*************************************************/
s8 RFID::PcdComMF522(u8 Command, u8* pDataIn, u8 InLenByte, u8* pDataOut, u16 *pOutLenBit){
	s8 status = MI_ERR;
	u8 irqEn = 0x00;
	u8 waitFor = 0x00;
	u8 lastBits;
	u8 n;
	u16 i;
	switch(Command){
	case PCD_AUTHENT:
		irqEn = 0x12;
		waitFor = 0x10;
		break;
	case PCD_TRANSCEIVE:
		irqEn = 0x77;
		waitFor = 0x30;// 接受到数据及命令执行完毕
		break;
	default:
		break;
	}
	writeRawRC(ComIEnReg, irqEn | 0x80);// 容许除定时器中断请求以为得所有中断请求
	clearBitMask(ComIrqReg, 0x80);// 屏蔽位清除
	writeRawRC(CommandReg, PCD_IDLE);// 取消当前命令
	setBitMask(FIFOLevelReg, 0x80);// 清除FIFO中的读写指针

	for(i = 0; i < InLenByte; i++){
		writeRawRC(FIFODataReg, pDataIn[i]);//数据写入FIFO
	}
	writeRawRC(CommandReg, Command);//写入命令,将缓冲区中的数据发送到天线,并激活自动接收器

	if(Command == PCD_TRANSCEIVE){//如果命令为0C
		setBitMask(BitFramingReg, 0x80);//相当于启动发送STARTSENG
	}
	i = 3000;//根据时钟频率调整，操作M1卡最大等待时间=600,操作IC卡最大等待时间=1200
	do{
		n = readRawRC(ComIrqReg);//读取中断标志,检查数据返回
		i--;
	}while((i != 0) &&!(n &0x01) &&!(n &waitFor));// 定时器未超时,没有错误,0x01,0x30
	clearBitMask(BitFramingReg, 0x80);// 相当于清除发送STARTSENG

	if(i != 0){// 定时时间到，i，没有递减到0
		if(!(readRawRC(ErrorReg) &0x1B)){// 判断有无出现错误标志	 Buffer溢出,位冲突,接收CRC错误,奇偶校验错误,
			status = MI_OK;// 初始化状态
			if(n &irqEn &0x01){// 若是PCD_TRANSCEIVE, irq = 0x77,  定时器为0中断产生,定时器为0时为错误
				status = MI_NOTAGERR;// 搜索不到卡
			}
			if(Command == PCD_TRANSCEIVE){// 如果是发送接收指令
				n = readRawRC(FIFOLevelReg);// 读取接收到的数据的字节数
				lastBits = readRawRC(ControlReg) &0x07;// 2-0:RxLastBits,显示接收到最后一个字节的位数
				if(lastBits){// 若该位为0，最后整个字节有效
					*pOutLenBit =(n - 1) *8 + lastBits;//pOutLenBit记录总共收到的位数
				}else{
					*pOutLenBit = n *8;//接收完整位数
				}
				if(n == 0){//假如没有中断产生
					n = 1;//n置1
				}
				if(n > MAXRLEN){// 一次最大能接受到的字节数
					n = MAXRLEN;//超出最大长度,只接受最大长度的值
				}
				for(i = 0; i < n; i++){
					pDataOut[i] = readRawRC(FIFODataReg);//从FIFO读取数据
				}
			}
		}else{
			status = MI_ERR;//有错误
		}
	}
	setBitMask(ControlReg, 0x80);//停止定时器
	writeRawRC(CommandReg, PCD_IDLE);//清空指令
	return status;//返回状态
}

/*************************************************
Function: RFID::PcdReset
Description: 复位RC522
Input: void
Return: 错误码 0成功
*************************************************/
s8 RFID::PcdReset(void){
	//外部复位
	*RST->O=1;
	delay_us(1);
	*RST->O=0;
	delay_us(1);
	*RST->O=1;
	delay_us(1);
	//软复位
	writeRawRC(CommandReg, PCD_RESETPHASE);
	//Timer: TPrescaler*TreloadVal/6.78MHz = 0xD3E*0x32/6.78=25ms
	writeRawRC(TModeReg, 0x8D);//TAuto=1为自动计数模式，受通信协议影响。低4位为预分频值的高4位
	//writeRawRC(TModeReg,0x1D);//TAutoRestart=1为自动重载计时，0x0D3E是0.5ms的定时初值//test
	writeRawRC(TPrescalerReg, 0x3E);//预分频值的低8位
	writeRawRC(TReloadRegL, 0x32);//计数器的低8位
	writeRawRC(TReloadRegH, 0x00);//计数器的高8位
	writeRawRC(TxAutoReg, 0x40);//必须要,设置逻辑1,强制100%ASK调制
	writeRawRC(ModeReg, 0x3D);//CRC初始值0x6363
	writeRawRC(CommandReg, 0x00);//启动MFRC522
	writeRawRC(RFCfgReg, 0x7F);//接收器增益.0x0F:18dB; 0x4F:33dB; 0x7F:48dB
	return MI_OK;//定时器时间6.78M/TPrescaler(ms)
}

/*************************************************
Function: RFID::CalulateCRC
Description: 用MF522计算CRC16函数
Input: 
	pIndata  需要计算的数据
	len      数据长度
	pDataOut 输出crc16的两个字节数组
Return: void
*************************************************/
void RFID::CalulateCRC(u8* pIndata, u8 len, u8* pDataOut){
	u8 i, n;
	clearBitMask(DivIrqReg, 0x04);
	writeRawRC(CommandReg, PCD_IDLE);//取消当前命令
	setBitMask(FIFOLevelReg, 0x80);//FlushBuffer 清除ErrReg 的标志位
	for(i = 0; i < len; i++){
		writeRawRC(FIFODataReg, *(pIndata + i));
	}
	writeRawRC(CommandReg, PCD_CALCCRC);
	i = 0xFF;
	do{
		n = readRawRC(DivIrqReg);
		i--;
	}while((i != 0) &&!(n &0x04));//当CRCIRq 所有数据被处理完毕该位置位
	pDataOut[0] = readRawRC(CRCResultRegL);
	pDataOut[1] = readRawRC(CRCResultRegM);
}

/*************************************************
Function: RFID::PcdHalt
Description: 命令卡片进入休眠状态
Input: void
Return: 错误码 0成功
*************************************************/
s8 RFID::PcdHalt(void){
	s8 status = MI_ERR;
	u16 unLen;
	u8 ucComMF522Buf[MAXRLEN];

	ucComMF522Buf[0] = PICC_HALT;
	ucComMF522Buf[1] = 0;
	CalulateCRC(ucComMF522Buf, 2, &ucComMF522Buf[2]);

	status = PcdComMF522(PCD_TRANSCEIVE, ucComMF522Buf, 4, ucComMF522Buf, &unLen);

	return status;
}

/*************************************************
Function: RFID::PcdRequest
Description: 寻卡
Input: 
	req_code  寻卡方式
		0x52 寻感应区内所有符合14443A标准的卡
		0x26 寻未进入休眠状态的卡
	pTagType  卡片类型代码
		0x4400 Mifare_UltraLight
		0x0400 Mifare_One(S50)
		0x0200 Mifare_One(S70)
		0x0800 Mifare_Pro(X)CPU卡
		0x4403 Mifare_DESFire
Return: 错误码 0成功
*************************************************/
s8 RFID::PcdRequest(u8 req_code, u8* pTagType){
	s8 status = MI_ERR;
	u16 unLen;
	u8 ucComMF522Buf[MAXRLEN];

	/*清空，做准备工作*/
	PcdReset();
	clearBitMask(Status2Reg, 0x08);// 清空校验成功标志,清除MFCrypto1On位
	writeRawRC(BitFramingReg, 0x07);// StartSend =0;RxAlign=0定义最后一个字节发送的位数,发送7个位
	setBitMask(TxControlReg, 0x03);// 两天线发射信号,Tx1RFEn,Tx2RFEn置1
	ucComMF522Buf[0] = req_code;//寻卡方式,所有卡还是其他什么卡
	status = PcdComMF522(PCD_TRANSCEIVE, ucComMF522Buf, 1, ucComMF522Buf, &unLen);// 将收到的卡片类型号保存
	if(status == MI_OK){
		*pTagType = ucComMF522Buf[0];
		*(pTagType + 1) = ucComMF522Buf[1];
	}else{
		status = MI_ERR;
	}
	return status;//返回结果
}

/*************************************************
Function: RFID::PcdAnticoll
Description: 防冲撞
Input: pSnr  卡片序列号，4字节
Return: 错误码 0成功
*************************************************/
s8 RFID::PcdAnticoll(u8* pSnr){
	s8 status;
	u8 i, snr_check = 0;
	u16 unLen;
	u8 ucComMF522Buf[MAXRLEN];

	clearBitMask(Status2Reg, 0x08);// 清空校验成功标志
	writeRawRC(BitFramingReg, 0x00);// 最后一个字节发送所有数据
	clearBitMask(CollReg, 0x80);// CollRegCollReg 0冲突结束后冲突位被置零
	ucComMF522Buf[0] = PICC_ANTICOLL1;// 防冲突指令，所有位在接收到冲突后将清除
	ucComMF522Buf[1] = 0x20;// 发送4个字节
	status = PcdComMF522(PCD_TRANSCEIVE, ucComMF522Buf, 2, ucComMF522Buf, &unLen);
	if(status == MI_OK){
		for(i = 0; i < 4; i++){// 接受卡ID
			*(pSnr + i) = ucComMF522Buf[i];// 0-3:ID
			snr_check ^= ucComMF522Buf[i];// 校验ID
		}
		if(snr_check != ucComMF522Buf[i]){// 4:校验值
			status = MI_ERR;// 校验出错
		}
	}else{
		status = MI_ERR;
	}
	setBitMask(CollReg, 0x80);// CollRegCollReg	在106kbps良好的防冲突情况下该位置1
	return status;// 返回结果
}

/*************************************************
Function: RFID::PcdSelect
Description: 选择卡片
Input: pSnr  卡片序列号，4字节
Return: 错误码 0成功
*************************************************/
s8 RFID::PcdSelect(u8* pSnr){
	s8 status;
	u8 i;
	u16 unLen;
	u8 ucComMF522Buf[MAXRLEN];

	clearBitMask(Status2Reg, 0x08);// 清空校验成功标志
	ucComMF522Buf[0] = PICC_ANTICOLL1;// 防冲突
	ucComMF522Buf[1] = 0x70;// 发送7字节
	ucComMF522Buf[6] = 0;//ID校验清0
	for(i = 0; i < 4; i++){
		ucComMF522Buf[i + 2] = *(pSnr + i);// 保存卡ID
		ucComMF522Buf[6] ^= *(pSnr + i);// 计算校验值
	}
	CalulateCRC(ucComMF522Buf, 7, &ucComMF522Buf[7]);// 生成发送内容的CRC校验,保存到最后两个字节
	status = PcdComMF522(PCD_TRANSCEIVE, ucComMF522Buf, 9, ucComMF522Buf, &unLen);// 发送选卡,及CRC校验
	if((status != MI_OK) ||(unLen != 0x18)){// 返回结果正确, 并且长度为24位, 3字节,(1)卡内存储器大小+(2)CRC
		status = MI_ERR;// 错误
	}
	return status;// 返回结果
}

/*************************************************
Function: RFID::GetCard
Description: IC卡检测,并返回数据
Input: 
	req_code  寻卡方式
		0x52 寻感应区内所有符合14443A标准的卡
		0x26 寻未进入休眠状态的卡
	pSnr  卡片序列号，4字节
	pTagType  卡片类型代码
		0x4400 Mifare_UltraLight
		0x0400 Mifare_One(S50)
		0x0200 Mifare_One(S70)
		0x0800 Mifare_Pro(X)CPU卡
		0x4403 Mifare_DESFire
Return: 错误码 0成功
*************************************************/
s8 RFID::GetCard(u8 Reqcode,u8* pSnr,u8* type){
	s8 status = MI_OK;
	
	if(pSnr == 0){pSnr = cardSN;}
	if(type == 0){type = cardType;}
	mem_set(pSnr, 0x00, 4);
	mem_set(type, 0x00, 2);
	status = PcdRequest(Reqcode, type);//寻卡
	if(status){
		return MI_NOCARD;
	}
	status = PcdAnticoll(pSnr);//防冲撞, 获取ID
	if(status){
		return MI_NOID;
	}
	status = PcdSelect(pSnr);//选择卡片
	if(status){
		return MI_NOSELECT;
	}
	return MI_OK;
}

/*************************************************
Function: RFID::S50Auth
Description: 验证卡片密码
Input: 
	mode  密码验证模式 0x60验证A密钥 0x61验证B密钥
	addr  块地址
	pKey  密码
	pSnr  卡片序列号，4字节
Return: 错误码 0成功
*************************************************/
s8 RFID::S50Auth(u8 mode,u8 addr,u8* pKey,u8* pSnr){
	s8  status;
	u16 unLen;
	u8  ucComMF522Buf[MAXRLEN]; 

	if(pSnr == 0){pSnr = cardSN;}
	ucComMF522Buf[0] = mode;
	ucComMF522Buf[1] = addr;
	mem_cpy(&ucComMF522Buf[2], pKey, 6);
	mem_cpy(&ucComMF522Buf[8], pSnr, 4);

	status = PcdComMF522(PCD_AUTHENT,ucComMF522Buf,12,ucComMF522Buf,&unLen);
	if((status != MI_OK) ||(!(readRawRC(Status2Reg) & 0x08))){
		return MI_ERRPWD;
	}else{
		return MI_OK;
	}
}

/*************************************************
Function: RFID::S50Read
Description: 读取M1卡一块数据
Input: 
	addr   块地址
	pData  读出的数据，16字节
	len    数据长度
Return: 错误码 0成功
*************************************************/
s8 RFID::S50Read(u8 addr,u8 *pData,u8 len){
	s8 status;
	u16 unLen;
	u8 i,ucComMF522Buf[MAXRLEN]; 

	mem_set(pData, 0x00, len);
	ucComMF522Buf[0] = PICC_READ;
	ucComMF522Buf[1] = addr;
	CalulateCRC(ucComMF522Buf,2,&ucComMF522Buf[2]);

	status = PcdComMF522(PCD_TRANSCEIVE,ucComMF522Buf,4,ucComMF522Buf,&unLen);
	if((status == MI_OK) &&(unLen == 0x90)){
		for(i=0; i<len; i++){
			*(pData+i) = ucComMF522Buf[i];
		}
	}else{
		status = MI_ERR;
	}
	return status;
}

/*************************************************
Function: RFID::S50Write
Description: 写数据到M1卡一块
Input: 
	addr   块地址
	pData  写入的数据，16字节
Return: 错误码 0成功
*************************************************/
s8 RFID::S50Write(u8 addr,u8 *pData){
	s8 status;
	u16 unLen;
	u8 i,ucComMF522Buf[MAXRLEN]; 
	
	ucComMF522Buf[0] = PICC_WRITE;
	ucComMF522Buf[1] = addr;
	CalulateCRC(ucComMF522Buf,2,&ucComMF522Buf[2]);
	status = PcdComMF522(PCD_TRANSCEIVE,ucComMF522Buf,4,ucComMF522Buf,&unLen);
	if((status != MI_OK) ||(unLen != 4) ||((ucComMF522Buf[0] & 0x0F) != 0x0A)){
		status = MI_ERR;
	}
	if(status == MI_OK){
		for(i=0; i<16; i++){
			ucComMF522Buf[i] = *(pData+i);
		}
		CalulateCRC(ucComMF522Buf,16,&ucComMF522Buf[16]);
		status = PcdComMF522(PCD_TRANSCEIVE,ucComMF522Buf,18,ucComMF522Buf,&unLen);
		if((status != MI_OK) ||(unLen != 4) ||((ucComMF522Buf[0] & 0x0F) != 0x0A)){
			status = MI_ERR;
		}
	}
	return status;
}

/*************************************************
Function: RFID::S50Value
Description: 扣款和充值
Input: 
	mode   命令字  0xC0扣款  0xC1充值
	addr   钱包地址
	pValue 4字节增(减)值，低位在前
Return: 错误码 0成功
*************************************************/
s8 RFID::S50Value(u8 dd_mode,u8 addr,u8 *pValue){
	s8 status;
	u16 unLen;
	u8 ucComMF522Buf[MAXRLEN]; 
	
	ucComMF522Buf[0] = dd_mode;
	ucComMF522Buf[1] = addr;
	CalulateCRC(ucComMF522Buf,2,&ucComMF522Buf[2]);

	status = PcdComMF522(PCD_TRANSCEIVE,ucComMF522Buf,4,ucComMF522Buf,&unLen);
	if((status != MI_OK) ||(unLen != 4) ||((ucComMF522Buf[0] & 0x0F) != 0x0A)){
		status = MI_ERR;
	}
	if(status == MI_OK){
		mem_cpy(ucComMF522Buf, pValue, 4);
		CalulateCRC(ucComMF522Buf,4,&ucComMF522Buf[4]);
		unLen = 0;
		status = PcdComMF522(PCD_TRANSCEIVE,ucComMF522Buf,6,ucComMF522Buf,&unLen);
		if(status != MI_ERR){
			status = MI_OK;
		}
	}
	if(status == MI_OK){
		ucComMF522Buf[0] = PICC_TRANSFER;
		ucComMF522Buf[1] = addr;
		CalulateCRC(ucComMF522Buf,2,&ucComMF522Buf[2]); 

		status = PcdComMF522(PCD_TRANSCEIVE,ucComMF522Buf,4,ucComMF522Buf,&unLen);

		if((status != MI_OK) ||(unLen != 4) ||((ucComMF522Buf[0] & 0x0F) != 0x0A)){
			status = MI_ERR;
		}
	}
	return status;
}


/*************************************************
Function: RFID::S50BakValue
Description: 备份钱包
Input: 
	sourceaddr 源地址
	goaladdr   目标地址
Return: 错误码 0成功
*************************************************/
s8 RFID::S50BakValue(u8 sourceaddr, u8 goaladdr){
	s8 status;
	u16 unLen;
	u8 ucComMF522Buf[MAXRLEN]; 

	ucComMF522Buf[0] = PICC_RESTORE;
	ucComMF522Buf[1] = sourceaddr;
	CalulateCRC(ucComMF522Buf,2,&ucComMF522Buf[2]);

	status = PcdComMF522(PCD_TRANSCEIVE,ucComMF522Buf,4,ucComMF522Buf,&unLen);
	if((status != MI_OK) ||(unLen != 4) ||((ucComMF522Buf[0] & 0x0F) != 0x0A)){
		status = MI_ERR;
	}
	if(status == MI_OK){
		ucComMF522Buf[0] = 0;
		ucComMF522Buf[1] = 0;
		ucComMF522Buf[2] = 0;
		ucComMF522Buf[3] = 0;
		CalulateCRC(ucComMF522Buf,4,&ucComMF522Buf[4]);
		status = PcdComMF522(PCD_TRANSCEIVE,ucComMF522Buf,6,ucComMF522Buf,&unLen);
		if(status != MI_ERR){
			status = MI_OK;
		}
	}
	if(status != MI_OK){
		return MI_ERR;
	}
	ucComMF522Buf[0] = PICC_TRANSFER;
	ucComMF522Buf[1] = goaladdr;
	CalulateCRC(ucComMF522Buf,2,&ucComMF522Buf[2]);
	status = PcdComMF522(PCD_TRANSCEIVE,ucComMF522Buf,4,ucComMF522Buf,&unLen);
	if((status != MI_OK) ||(unLen != 4) ||((ucComMF522Buf[0] & 0x0F) != 0x0A)){
		status = MI_ERR;
	}
	return status;
}
