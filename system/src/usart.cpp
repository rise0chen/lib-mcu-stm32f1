#include "usart.hpp"

usart usart1(1);
usart usart2(2);
usart usart3(3);
extern void USART1_Do(void);
extern void USART2_Do(void);
extern void USART3_Do(void);

usart::usart(u8 t){
	if(t==1){
		the = USART1;
		RCC_GPIO = APB2_GPIOA;
		RCC_The	= APB2_USART1;
		Px	= PA;
		PTX = 9;
		PRX = 10;
		IRQn = USART1_IRQn;
		TX_DMA = DMA1_Channel4;
		RX_DMA = DMA1_Channel5;
		FLAG_TC=0x00002000;
		funRx = USART1_Do;
	}
	if(t==2){
		the = USART2;
		RCC_GPIO = APB2_GPIOA;
		RCC_The	= APB1_USART2;
		Px	= PA;
		PTX = 2;
		PRX = 3;
		IRQn = USART2_IRQn;
		TX_DMA = DMA1_Channel7;
		RX_DMA = DMA1_Channel6;
		FLAG_TC=0x02000000;
		funRx = USART2_Do;
	}
	if(t==3){
		the = USART3;
		RCC_GPIO = APB2_GPIOB;
		RCC_The	= APB1_USART3;
		Px	= PB;
		PTX = 10;
		PRX = 11;
		IRQn = USART3_IRQn;
		TX_DMA = DMA1_Channel2;
		RX_DMA = DMA1_Channel3;
		FLAG_TC=0x00000020;
		funRx = USART3_Do;
	}
}
void usart::Config(u32 bound,u8 s,u8 e){
	u32 clk, temp;
	u16 integer, fraction;

	start=s;//起始标志
	end=e;//结束标志
	RCC->APB2ENR	|= RCC_GPIO;//使能PORT口时钟
//使能串口时钟
	if(the==USART1){
		rcc::Reset(2,RCC_The);
		RCC->APB2ENR	|= RCC_The;
		clk = apb2clk;
	}else{
		rcc::Reset(1,RCC_The);
		RCC->APB1ENR	|= RCC_The;
		clk = apb1clk;
	}
	//GPIO端口设置
	gpio(Px, PTX).Config(P_ODAF, 1, P_50MHz);//TX推挽输出
	gpio(Px, PRX).Config(P_FIN);//RX浮空输入
	//波特率设置
	integer=clk/(bound<<4);//得到整数部分
	temp=(clk<<4)/bound;//得到USARTDIV
	fraction=(temp-(integer<<8))>>4;//得到小数部分
	the->BRR=(integer<<4)+fraction;// 波特率设置
	//使能接收中断
	the->CR1|=1<<5;//RXNE(1<<6:IDLE)中断使能
	nvic::Init(IRQn,2,0);
	//DMA设置
	dma::TxConfig(TX_DMA,(u32)&the->DR,(u32)&TX_BUF,1);
	//dma::Rx_Config(RX_DMA,(u32)&the->DR,(u32)&RX_BUF,USART_LEN);
	the->CR3 |= 0xC0;//DMA使能
	the->CR1|=0X200C;//使能,8位数据,无校验位,1位停止,收发
}
void usart::printf(char *format, ...){
	va_list ap;
	va_start(ap,format);
	while((DMA1->ISR & FLAG_TC)==0);//等待上次结束
	TX_DMA->CCR &= ~1;//关DMA
	vsprintf(TX_BUF, format, ap );
	TX_DMA->CNDTR=std::strlen(TX_BUF);
	DMA1->IFCR |= FLAG_TC;//清TC中断
	TX_DMA->CCR |= 1;//开DMA
}
void usart::Send(char *buf, u16 len){
	if(len==0){len=std::strlen(buf);}
	while((DMA1->ISR & FLAG_TC)==0);//等待上次结束
	TX_DMA->CCR &= ~1;//关DMA
	std::memcpy(TX_BUF, buf, len);
	TX_DMA->CNDTR=len;
	DMA1->IFCR |= FLAG_TC;//清TC中断
	TX_DMA->CCR |= 1;//开DMA
}
void usart::Receive(){
	u8	res;

	if(the->SR&(1<<5)){
		res=the->DR;
		if((RX_STA>>6)!=3){//未开始
			if(start==0x00 && res!=0x00){//无开始标志
				RX_STA|=0xC0;
				RX_BUF[RX_Len++]=res;
			}else if(start!=0x00 && res==start){//开始
				RX_STA|=0xC0;
				RX_BUF[RX_Len++]=res;
			}
		}else if((RX_STA>>4)!=0xf){//未结束
			RX_BUF[RX_Len++]=res;
			if(RX_Len >= USART_LEN-8){
				RX_STA=0;RX_Len=0;//数据过多
			}
			if(res==end){//结束
				if(end==0x0D){//0x0D 0x0A 结尾
					RX_STA|=0x20;
				}else{
					RX_STA|=0x30;
					RX_BUF[RX_Len]=0x00;
					(*funRx)();//执行it.c中的函数
					RX_STA=0;RX_Len=0;
				}
			}else if((RX_STA>>4) == 0x0E){
				if(res==0x0A){
					RX_STA|=0x30;
					RX_BUF[RX_Len]=0x00;
					(*funRx)();//执行it.c中的函数
					RX_STA=0;RX_Len=0;
				}
			}
		}else{
			RX_STA=0;RX_Len=0;
		}
	}
}

_C void USART1_IRQHandler(void){
	usart1.Receive();
}
_C void USART2_IRQHandler(void){
	usart2.Receive();
}
_C void USART3_IRQHandler(void){
	usart3.Receive();
}
