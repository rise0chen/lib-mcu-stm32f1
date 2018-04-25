#include "RFID.hpp"

namespace RFID{
	spi *com = &spi2;
	gpio *RST = &gpio_default;
	gpio *CS = &gpio_default;
	u8 type[2];
	u8 pSnr[4];//IC卡ID


	void Init(){
		RST->Config(P_PPO);
		CS->Config(P_PPO);
		PcdReset();
		PcdAntennaOff();
		PcdAntennaOn();
	}
	
	/**********************************************************************
	* @brief  写RC522寄存器
	* @param  Address:寄存器地址
	*         value:写入数值
	* @retval 无
	**********************************************************************/
	static void WriteRawRC(u8 Address, u8 value){
		*CS->O=0;//选中
		//寄存器地址格式:0XXXXXX0
		com->ReadWriteByte((Address << 1) &0x7E);
		com->ReadWriteByte(value);
		*CS->O=1;//放开
	}
	/**********************************************************************
	* @brief  读RC522寄存器
	* @param  Address:寄存器地址
	* @retval 读出的值
	**********************************************************************/
	static u8 ReadRawRC(u8 Address){
		u8 data;

		*CS->O=0;//选中
		//寄存器地址格式:1XXXXXX0
		com->ReadWriteByte((Address << 1) | 0x80);
		data = com->ReadWriteByte(0x00);
		*CS->O=1;//放开
		return(data);
	}
	
	/**********************************************************************
	* @brief  置位
	* @param  reg:寄存器地址
	*         mask:置位值
	* @retval 无
	**********************************************************************/
	static void SetBitMask(u8 reg, u8 mask){
		u8 tmp = 0x0;
		tmp = ReadRawRC(reg);
		WriteRawRC(reg, tmp | mask);// set bit mask
	}
	/**********************************************************************
	* @brief  复位
	* @param  reg:寄存器地址
	*         mask:置位值
	* @retval 无
	**********************************************************************/
	static void ClearBitMask(u8 reg, u8 mask){
		u8 tmp = 0x00;
		tmp = ReadRawRC(reg);
		WriteRawRC(reg, tmp &~mask);// clear bit mask
	}
	
	/**********************************************************************
	* @brief  关闭天线
	* @param  无
	* @retval 无
	**********************************************************************/
	void PcdAntennaOff(void){
		ClearBitMask(TxControlReg, 0x03);//禁止Tx1RFEn,Tx2RFEn
	}
	/**********************************************************************
	* @brief  开启天线,每次启动或关闭天险发射之间应至少有1ms的间隔
	* @param  无
	* @retval 无
	**********************************************************************/
	void PcdAntennaOn(void){
		u8 i;
		i = ReadRawRC(TxControlReg);//读取出发送控制寄存器
		if(!(i &0x03)){//如果未开启,则
			SetBitMask(TxControlReg, 0x03);//开启Tx1RFEn,Tx2RFEn
		}
	}
	/**********************************************************************
	* @brief  通过RC522和ISO14443卡通讯
	* @param  Command:RC522命令字
	*         pDataIn:通过RC522发送到卡片的数据
	*         InLenByte:发送数据的字节长度
	* @retval pDataOut:接收到的卡片返回数据
	*         pOutLenBit:返回数据的位长度
	**********************************************************************/
	s8 PcdComMF522(u8 Command, u8* pDataIn, u8 InLenByte, u8* pDataOut, u16 *pOutLenBit){
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
		WriteRawRC(ComIEnReg, irqEn | 0x80);// 容许除定时器中断请求以为得所有中断请求
		ClearBitMask(ComIrqReg, 0x80);// 屏蔽位清除
		WriteRawRC(CommandReg, PCD_IDLE);// 取消当前命令
		SetBitMask(FIFOLevelReg, 0x80);// 清除FIFO中的读写指针

		for(i = 0; i < InLenByte; i++){
			WriteRawRC(FIFODataReg, pDataIn[i]);//数据写入FIFO
		}
		WriteRawRC(CommandReg, Command);//写入命令,将缓冲区中的数据发送到天线,并激活自动接收器

		if(Command == PCD_TRANSCEIVE){//如果命令为0C
			SetBitMask(BitFramingReg, 0x80);//相当于启动发送STARTSENG
		}
		i = 3000;//根据时钟频率调整，操作M1卡最大等待时间=600,操作IC卡最大等待时间=1200
		do{
			n = ReadRawRC(ComIrqReg);//读取中断标志,检查数据返回
			i--;
		}while((i != 0) &&!(n &0x01) &&!(n &waitFor));// 定时器未超时,没有错误,0x01,0x30
		ClearBitMask(BitFramingReg, 0x80);// 相当于清除发送STARTSENG

		if(i != 0){// 定时时间到，i，没有递减到0
			if(!(ReadRawRC(ErrorReg) &0x1B)){// 判断有无出现错误标志	 Buffer溢出,位冲突,接收CRC错误,奇偶校验错误,
				status = MI_OK;// 初始化状态
				if(n &irqEn &0x01){// 若是PCD_TRANSCEIVE, irq = 0x77,  定时器为0中断产生,定时器为0时为错误
					status = MI_NOTAGERR;// 搜索不到卡
				}
				if(Command == PCD_TRANSCEIVE){// 如果是发送接收指令
					n = ReadRawRC(FIFOLevelReg);// 读取接收到的数据的字节数
					lastBits = ReadRawRC(ControlReg) &0x07;// 2-0:RxLastBits,显示接收到最后一个字节的位数
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
						pDataOut[i] = ReadRawRC(FIFODataReg);//从FIFO读取数据
					}
				}
			}else{
				status = MI_ERR;//有错误
			}
		}
		SetBitMask(ControlReg, 0x80);//停止定时器
		WriteRawRC(CommandReg, PCD_IDLE);//清空指令
		return status;//返回状态
	}
	
	/**********************************************************************
	* @brief  复位RC522
	* @param  无
	* @retval 成功返回MI_OK
	**********************************************************************/
	s8 PcdReset(void){
		//外部复位
		*RST->O=1;
		delay_us(1);
		*RST->O=0;
		delay_us(1);
		*RST->O=1;
		delay_us(1);
		//软复位
		WriteRawRC(CommandReg, PCD_RESETPHASE);
		//Timer: TPrescaler*TreloadVal/6.78MHz = 0xD3E*0x32/6.78=25ms
		WriteRawRC(TModeReg, 0x8D);//TAuto=1为自动计数模式，受通信协议影响。低4位为预分频值的高4位
		//WriteRawRC(TModeReg,0x1D);//TAutoRestart=1为自动重载计时，0x0D3E是0.5ms的定时初值//test
		WriteRawRC(TPrescalerReg, 0x3E);//预分频值的低8位
		WriteRawRC(TReloadRegL, 0x32);//计数器的低8位
		WriteRawRC(TReloadRegH, 0x00);//计数器的高8位
		WriteRawRC(TxAutoReg, 0x40);//必须要,设置逻辑1,强制100%ASK调制
		WriteRawRC(ModeReg, 0x3D);//CRC初始值0x6363
		WriteRawRC(CommandReg, 0x00);//启动MFRC522
		WriteRawRC(RFCfgReg, 0x7F);//接收器增益.0x0F:18dB; 0x4F:33dB; 0x7F:48dB
		return MI_OK;//定时器时间6.78M/TPrescaler(ms)
	}
	
	/**********************************************************************
	* @brief  用MF522计算CRC16函数
	* @param  pIndata:需要计算的数据
	*         len:数据长度
	* @retval pDataOut:输出结果的两个字节数组
	**********************************************************************/
	void CalulateCRC(u8* pIndata, u8 len, u8* pDataOut){
		u8 i, n;
		ClearBitMask(DivIrqReg, 0x04);
		WriteRawRC(CommandReg, PCD_IDLE);//取消当前命令
		SetBitMask(FIFOLevelReg, 0x80);//FlushBuffer 清除ErrReg 的标志位
		for(i = 0; i < len; i++){
			WriteRawRC(FIFODataReg, *(pIndata + i));
		}
		WriteRawRC(CommandReg, PCD_CALCCRC);
		i = 0xFF;
		do{
			n = ReadRawRC(DivIrqReg);
			i--;
		}while((i != 0) &&!(n &0x04));//当CRCIRq 所有数据被处理完毕该位置位
		pDataOut[0] = ReadRawRC(CRCResultRegL);
		pDataOut[1] = ReadRawRC(CRCResultRegM);
	}
	
	/**********************************************************************
	* @brief  命令卡片进入休眠状态
	* @param  无
	* @retval 成功返回MI_OK
	**********************************************************************/
	s8 PcdHalt(void){
		s8 status = MI_ERR;
		u16 unLen;
		u8 ucComMF522Buf[MAXRLEN];

		ucComMF522Buf[0] = PICC_HALT;
		ucComMF522Buf[1] = 0;
		CalulateCRC(ucComMF522Buf, 2, &ucComMF522Buf[2]);

		status = PcdComMF522(PCD_TRANSCEIVE, ucComMF522Buf, 4, ucComMF522Buf, &unLen);

		return status;
	}
	
	/**********************************************************************
	* @brief  寻卡
	* @param  req_code:寻卡方式, 0x52 = 寻感应区内所有符合14443A标准的卡,0x26 = 寻未进入休眠状态的卡
	* @retval pTagType：卡片类型代码
	*             0x4400 = Mifare_UltraLight
	*             0x0400 = Mifare_One(S50)
	*             0x0200 = Mifare_One(S70)
	*             0x0800 = Mifare_Pro(X)CPU卡
	*             0x4403 = Mifare_DESFire
	**********************************************************************/
	s8 PcdRequest(u8 req_code, u8* pTagType){
		s8 status = MI_ERR;
		u16 unLen;
		u8 ucComMF522Buf[MAXRLEN];

		/*清空，做准备工作*/
		PcdReset();
		ClearBitMask(Status2Reg, 0x08);// 清空校验成功标志,清除MFCrypto1On位
		WriteRawRC(BitFramingReg, 0x07);// StartSend =0;RxAlign=0定义最后一个字节发送的位数,发送7个位
		SetBitMask(TxControlReg, 0x03);// 两天线发射信号,Tx1RFEn,Tx2RFEn置1
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
	
	/**********************************************************************
	* @brief  防冲撞
	* @param  pSnr:卡片序列号，4字节
	* @retval 成功返回MI_OK
	**********************************************************************/
	s8 PcdAnticoll(u8* pSnr){
		s8 status;
		u8 i, snr_check = 0;
		u16 unLen;
		u8 ucComMF522Buf[MAXRLEN];

		ClearBitMask(Status2Reg, 0x08);// 清空校验成功标志
		WriteRawRC(BitFramingReg, 0x00);// 最后一个字节发送所有数据
		ClearBitMask(CollReg, 0x80);// CollRegCollReg 0冲突结束后冲突位被置零
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
		SetBitMask(CollReg, 0x80);// CollRegCollReg	在106kbps良好的防冲突情况下该位置1
		return status;// 返回结果
	}
	
	/**********************************************************************
	* @brief  选择卡片
	* @param  pSnr:卡片序列号，4字节
	*         
	* @retval 成功返回MI_OK
	**********************************************************************/
	s8 PcdSelect(u8* pSnr){
		s8 status;
		u8 i;
		u16 unLen;
		u8 ucComMF522Buf[MAXRLEN];

		ClearBitMask(Status2Reg, 0x08);// 清空校验成功标志
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
	
	/**********************************************************************
	* @brief  IC卡检测,并返回数据
	* @param  req_code:寻卡方式, 0x52 = 寻感应区内所有符合14443A标准的卡,0x26 = 寻未进入休眠状态的卡
	* @retval psnr:IC卡ID
	*         pTagType：卡片类型代码
	*             0x4400 = Mifare_UltraLight
	*             0x0400 = Mifare_One(S50)
	*             0x0200 = Mifare_One(S70)
	*             0x0800 = Mifare_Pro(X)
	*             0x4403 = Mifare_DESFire
	**********************************************************************/
	s8 GetCard(u8 Reqcode,u8* pSnr,u8* type){
		s8 status = MI_OK;
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
	
	/**********************************************************************
	* @brief  验证卡片密码
	* @param  auth_mode[IN]: 密码验证模式
	*                0x60 = 验证A密钥
	*                0x61 = 验证B密钥
	*         addr[IN]：块地址
	*         pKey[IN]：密码
	* @retval 成功返回MI_OK
	**********************************************************************/
	s8 S50Auth(u8 auth_mode,u8 addr,u8* pKey,u8* pSnr){
		s8  status;
		u16 unLen;
		u8  ucComMF522Buf[MAXRLEN]; 

		ucComMF522Buf[0] = auth_mode;
		ucComMF522Buf[1] = addr;
		std::memcpy(&ucComMF522Buf[2], pKey, 6);
		std::memcpy(&ucComMF522Buf[8], pSnr, 4);

		status = PcdComMF522(PCD_AUTHENT,ucComMF522Buf,12,ucComMF522Buf,&unLen);
		if((status != MI_OK) ||(!(ReadRawRC(Status2Reg) & 0x08))){
			return MI_ERRPWD;
		}else{
			return MI_OK;
		}
	}
	
	/**********************************************************************
	* @brief  读取M1卡一块数据
	* @param  addr[IN]：块地址
	*         pData[OUT]：读出的数据，16字节
	* @retval 成功返回MI_OK
	**********************************************************************/
	s8 S50Read(u8 addr,u8 *pData,u8 len){
		s8 status;
		u16 unLen;
		u8 i,ucComMF522Buf[MAXRLEN]; 

		std::memset(pData, 0x00, len);
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
	
	/**********************************************************************
	* @brief  写数据到M1卡一块
	* @param  addr[IN]：块地址
	*         pData[IN]：写入的数据，16字节
	* @retval 成功返回MI_OK
	**********************************************************************/
	s8 S50Write(u8 addr,u8 *pData){
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
	
	/**********************************************************************
	* @brief  扣款和充值
	* @param  dd_mode[IN]：命令字
	*                0xC0 = 扣款
	*                0xC1 = 充值
	*         addr[IN]：钱包地址
	*         pValue[IN]：4字节增(减)值，低位在前
	* @retval 成功返回MI_OK
	**********************************************************************/
	s8 S50Value(u8 dd_mode,u8 addr,u8 *pValue){
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
			std::memcpy(ucComMF522Buf, pValue, 4);
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
	
	/**********************************************************************
	* @brief  备份钱包
	* @param  sourceaddr[IN]：源地址
	*         goaladdr[IN]：目标地址
	* @retval 成功返回MI_OK
	**********************************************************************/
	s8 S50BakValue(u8 sourceaddr, u8 goaladdr){
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
}
