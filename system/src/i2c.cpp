#include "i2c.hpp"

i2c i2c1(1);
i2c i2c2(2);

i2c::i2c(u8 t){
	if(t==1){
		the = I2C1;
		RCC_GPIO = APB2_GPIOB;
		RCC_The = APB1_I2C1;
		Px = PB;
		PSCL = 6;
		PSDA = 7;
		IRQn = I2C1_EV_IRQn;
	}
	if(t==2){
		the = I2C2;
		RCC_GPIO = APB2_GPIOB;
		RCC_The = APB1_I2C2;
		Px = PB;
		PSCL = 10;
		PSDA = 11;
		IRQn = I2C2_EV_IRQn;
	}
}
void i2c::config(void){
	#if I2C_SOFE//软件I2C
	gpio(Px, PSCL).config(P_ODO);
	gpio(Px, PSDA).config(P_ODO);
	*gpio(Px, PSCL).O=1;
	*gpio(Px, PSDA).O=1;
	#else//硬件I2C外设

	#endif
}
void i2c::start(void){
	#if I2C_SOFE//软件I2C
	gpio(Px, PSDA).config(P_ODO);
	*gpio(Px, PSCL).O=1; //起始条件：SCL线是高电平时，SDA 线从高电平向低电平切换
	*gpio(Px, PSDA).O=1;
	delay_us(5); //需要大于4.7us
	*gpio(Px, PSDA).O=0;
	delay_us(5); //需要大于4us
	*gpio(Px, PSCL).O=0;
	delay_us(1);
	#else//硬件I2C外设

	#endif
}
void i2c::stop(void){
	#if I2C_SOFE//软件I2C
	gpio(Px, PSDA).config(P_ODO);
	*gpio(Px, PSCL).O=0;
	*gpio(Px, PSDA).O=0;
	delay_us(1);
	*gpio(Px, PSCL).O=1;//停止条件：SCL线是高电平时，SDA 线由低电平向高电平切换
	delay_us(1);
	*gpio(Px, PSDA).O=1;
	delay_us(1);
	#else//硬件I2C外设

	#endif
}
void i2c::ack(u8 en){
	#if I2C_SOFE//软件I2C
	*gpio(Px, PSCL).O=0;
	gpio(Px, PSDA).config(P_ODO);
	*gpio(Px, PSDA).O= !en;//SDA传输完前8位时，当SCL来到第9个高电平时，SDA线为低电平，则为有效应答位
	delay_us(1);
	*gpio(Px, PSCL).O=1;
	delay_us(1);
	*gpio(Px, PSCL).O=0;
	delay_us(1);
	#else//硬件I2C外设

	#endif
}
ErrorStatus i2c::waitAck(void){
	#if I2C_SOFE//软件I2C
	gpio(Px, PSDA).config(P_UIN);//SDA设置为输入
	*gpio(Px, PSDA).O=1;
	delay_us(1);
	*gpio(Px, PSCL).O=1;
	delay_us(1);
	
	reTry = 250;
	while(*gpio(Px, PSDA).I){
		if(reTry-- <= 0){stop();return OVERTIME;}
	}
	*gpio(Px, PSCL).O=0;
	return SUCCESS;
	#else//硬件I2C外设

	#endif
}
ErrorStatus i2c::write(u8 data){
	u8 i;

	gpio(Px, PSDA).config(P_ODO);
	*gpio(Px, PSCL).O=0;
	for(i=0;i<8;i++){//I2C总线在SCL线为高电平时传输数据，在低电平时更换数据
		if(data&0x80)
			*gpio(Px, PSDA).O=1;
		else
			*gpio(Px, PSDA).O=0;
		data<<=1;
		delay_us(1);
		*gpio(Px, PSCL).O=1;
		delay_us(1);//SCL线由高电平变为低电平时需要有0.3us的延迟
		*gpio(Px, PSCL).O=0;
		delay_us(1);//SCL线由低电平变为高电平时需要有0.3us的延迟
	}
	return(waitAck());
}
u8 i2c::read(u8 isAck){
	u8 i,receive = 0;

	gpio(Px, PSDA).config(P_UIN);
	for(i=0;i<8;i++){
		*gpio(Px, PSCL).O=0;
		delay_us(1);
		*gpio(Px, PSCL).O=1;
		receive<<=1;
		if(*gpio(Px, PSDA).I)
			receive++;
		delay_us(1);
	}
	*gpio(Px, PSCL).O=0;
	delay_us(1);
	ack(isAck);//发送ACK 或 NACK
	return(receive);
}

