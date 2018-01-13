#include "adc.hpp"

adc adc1(1);
adc adc2(2);
adc adc3(3);

adc::adc(u8 t){
	switch(t){
		case 1:
			the=ADC1;
			RCC_The=APB2_ADC1;
			break;
		case 2:
			the=ADC2;
			RCC_The=APB2_ADC2;
			break;
		case 3:
			the=ADC3;
			RCC_The=APB2_ADC3;
			break;
	}
}
void adc::Init(void){
	rcc::Cmd(2,RCC_The,ENABLE);//ADC时钟使能
	rcc::Reset(2,RCC_The);//ADC复位
	SetMem(&RCC->CFGR, 3<<14, 2<<14);//ADC分频。PCLK2/6=12MHz,ADC最大时钟不能超过14M，否则将导致ADC准确度下降!
	SetMem(&the->CR1, 0xF<<16, 0<<16);//独立工作模式  
	ClearBit(&the->CR1,8);//非扫描模式
	ClearBit(&the->CR2,1);//单次转换模式
	SetMem(&the->CR2, 7<<17, 7<<17);//软件控制转换
	SetBit(&the->CR2,20);//使用用外部触发(SWSTART)!!!	必须使用一个事件来触发
	ClearBit(&the->CR2,11);//右对齐
	SetMem(&the->SQR1,0XF<<20,0<<20);//1个转换在规则序列中
	the->SMPR2=0x3FFFFFFF;//239.5周期,提高采样时间可以提高精确度
	the->SMPR1=0x00FFFFFF;
	//SetMem(&the->SMPR2,7<<(3*1),7<<(3*1));
	SetBit(&the->CR2,0);//开启AD转换器
	SetBit(&the->CR2,3);//使能复位校准
	while(the->CR2 & (1<<3));//等待校准结束
	SetBit(&the->CR2,2);//开启AD校准
	while(the->CR2&1<<2);//等待校准结束
}
u16 adc::Get(u8 ch, u8 times){
	u32 temp_val=0;
	u8 t;
	for(t=0;t<times;t++){
		SetMem(&the->SQR3, 0x1F<<0, ch<<0);//规则序列1 通道ch
		the->CR2|=1<<22;//启动规则转换通道
		while(!(the->SR & 1<<1));//等待转换结束
		temp_val+=the->DR;
	}
	return(temp_val/times);
}
float adc::GetTemp(u8 times){
	u16 value;
	float temperate;
	
	SetBit(&adc1.the->CR2,23);
	value=adc1.Get(16,times);//通道16采集10次,取平均
	temperate=(float)value*3.3/4096;//电压值
	temperate=(1.43-temperate)/0.0043+25;//转换为温度值
	ClearBit(&adc1.the->CR2,23);
	return temperate;
}
