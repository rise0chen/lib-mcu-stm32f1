#include "usart.hpp"

usart usart1(1);
usart usart2(2);
usart usart3(3);
extern void USART1_Do(char* msg, u16 len);
extern void USART2_Do(char* msg, u16 len);
extern void USART3_Do(char* msg, u16 len);

usart::usart(u8 t){
	if(t==1){
		the = USART1;
		RCC_GPIO = APB2_GPIOA;
		RCC_The  = APB2_USART1;
		Px  = PA;
		PTX = 9;
		PRX = 10;
		IRQn = USART1_IRQn;
		TX_DMA = DMA1_Channel4;
		RX_DMA = DMA1_Channel5;
		FLAG_TC= 0x00002000;
		funRx  = USART1_Do;
	}
	if(t==2){
		the = USART2;
		RCC_GPIO = APB2_GPIOA;
		RCC_The  = APB1_USART2;
		Px  = PA;
		PTX = 2;
		PRX = 3;
		IRQn = USART2_IRQn;
		TX_DMA = DMA1_Channel7;
		RX_DMA = DMA1_Channel6;
		FLAG_TC= 0x02000000;
		funRx  = USART2_Do;
	}
	if(t==3){
		the = USART3;
		RCC_GPIO = APB2_GPIOB;
		RCC_The  = APB1_USART3;
		Px  = PB;
		PTX = 10;
		PRX = 11;
		IRQn = USART3_IRQn;
		TX_DMA = DMA1_Channel2;
		RX_DMA = DMA1_Channel3;
		FLAG_TC= 0x00000020;
		funRx  = USART3_Do;
	}
}
void usart::config(u32 bound,u8 s,u8 e){
	u32 clk, temp;
	u16 integer, fraction;

	rx = bufRcv(s, e, funRx);
	
	RCC->APB2ENR |= RCC_GPIO;//使能PORT口时钟
//使能串口时钟
	if(the==USART1){
		rcc::reset(2,RCC_The);
		rcc::cmd(2, RCC_The, ENABLE);
		clk = apb2clk;
	}else{
		rcc::reset(1,RCC_The);
		rcc::cmd(1, RCC_The, ENABLE);
		RCC->APB1ENR |= RCC_The;
		clk = apb1clk;
	}
	//GPIO端口设置
	gpio(Px, PTX).config(P_ODAF, 1, P_50MHz);//TX推挽输出
	gpio(Px, PRX).config(P_FIN);//RX浮空输入
	//波特率设置
	integer=clk/(bound<<4);//得到整数部分
	temp = (clk<<4)/bound;//得到USARTDIV
	fraction=(temp-(integer<<8))>>4;//得到小数部分
	the->BRR=(integer<<4)+fraction;// 波特率设置
	//使能接收中断
	the->CR1 |= 1<<5;//RXNE(1<<6:IDLE)中断使能
	nvic::init(IRQn,2,0);
	//DMA设置
	dma::configTx(TX_DMA,(u32)&the->DR,(u32)&tx.buf,1);
	//dma::configRx(RX_DMA,(u32)&the->DR,(u32)&rx.buf,USART_LEN);
	the->CR3 |= 0xC0;//DMA使能
	the->CR1 |= 0X200C;//使能,8位数据,无校验位,1位停止,收发
}
void usart::printf(char *format, ...){
	va_list ap;
	va_start(ap,format);
	while((DMA1->ISR & FLAG_TC)==0);//等待上次结束
	TX_DMA->CCR &= ~1;//关DMA
	vsprintf(tx.buf, format, ap );
	TX_DMA->CNDTR=std::strlen(tx.buf);
	DMA1->IFCR  |= FLAG_TC;//清TC中断
	TX_DMA->CCR |= 1;//开DMA
}
void usart::send(char *buf, u16 len){
	if(len==0){len=std::strlen(buf);}
	while((DMA1->ISR & FLAG_TC)==0);//等待上次结束
	TX_DMA->CCR &= ~1;//关DMA
	std::memcpy(tx.buf, buf, len);
	TX_DMA->CNDTR=len;
	DMA1->IFCR  |= FLAG_TC;//清TC中断
	TX_DMA->CCR |= 1;//开DMA
}
void usart::rcv(){
	u8 res;

	if(the->SR&(1<<5)){
		res=the->DR;
		rx.rcv(res);
	}
}

_C void USART1_IRQHandler(void){
	usart1.rcv();
}
_C void USART2_IRQHandler(void){
	usart2.rcv();
}
_C void USART3_IRQHandler(void){
	usart3.rcv();
}
