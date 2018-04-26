#include "spi.hpp"

spi spi1(1);
spi spi2(2);
extern void SPI1_Do(void);
extern void SPI2_Do(void);

spi::spi(u8 t){
	if(t==1){
		the = SPI1;
		RCC_GPIO = APB2_GPIOA;
		RCC_The = APB2_SPI1;
		Px = PA;
		PSCK = 5;
		PMISO= 6;
		PMOSI= 7;
		IRQn=SPI1_IRQn;
		TX_DMA = DMA1_Channel4;
		RX_DMA = DMA1_Channel5;
		FLAG_TC=0x00002000;
		funRx=SPI1_Do;
	}
	if(t==2){
		the = SPI2;
		RCC_GPIO = APB2_GPIOB;
		RCC_The = APB1_SPI2;
		Px = PB;
		PSCK = 13;
		PMISO= 14;
		PMOSI= 15;
		IRQn=SPI2_IRQn;
		TX_DMA = DMA1_Channel7;
		RX_DMA = DMA1_Channel6;
		FLAG_TC=0x02000000;
		funRx=SPI2_Do;
	}
}
void spi::config(){
	gpio SCK  = gpio(Px, PSCK);
	gpio MISO = gpio(Px, PMISO);
	gpio MOSI = gpio(Px, PMOSI);
	
	rcc::cmd(2, RCC_GPIO, ENABLE);//GPIO时钟
	if(the==SPI1){//SPI时钟使能
		rcc::cmd(1, RCC_The, ENABLE);
	}else{
		rcc::cmd(2, RCC_The, ENABLE);
	}
	SCK.config(P_PPAF);
	MISO.config(P_PPAF);
	MOSI.config(P_PPAF);
	//the->CR1=0x035F;
	the->CR1 |= 0<<10; //全双工模式
	the->CR1 |= 1<<9;  //软件nss管理
	the->CR1 |= 1<<8;
	the->CR1 |= 1<<2;  //SPI主机
	the->CR1 |= 0<<11; //8bit数据格式
	the->CR1 |= 1<<1;  //空闲模式下SCK为1 CPOL=1
	the->CR1 |= 1<<0;  //数据采样从第二个时间边沿开始,CPHA=1
	the->CR1 |= 7<<3;  //Fsck=Fpclk1/256
	the->CR1 |= 0<<7;  //MSBfirst
	the->CR1 |= 1<<6;  //SPI设备使能
	rwByte(0xff);//启动传输
}
void spi::setSpeed(u8 speed){
//SPI速度=fAPB/2^(SpeedSet+1)
	the->CR1&=0XFF87;//失能、清速度
	the->CR1 |= (speed&0X07)<<3;//设置速度
	the->CR1 |= 1<<6;//SPI设备使能
}
u8 spi::rwByte(u8 data){
//TxData:要写入的字节
//返回值:读取到的字节
	reTry = 0XFFFE;
	while((the->SR&1<<1)==0){//等待发送区空
		if(reTry-- <= 0)return 0xFF;//超时退出
	}
	the->DR = data;//发送一个byte
	
	reTry = 0XFFFE;
	while((the->SR&1<<0)==0){//等待接收完一个byte
		if(reTry-- <= 0)return 0xFF;//超时退出
	}
	return the->DR;//返回收到的数据
}
