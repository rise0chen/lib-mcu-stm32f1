/*************************************************
Copyright (C), 2018-2028, Crise Tech. Co., Ltd.
File name: rtc.cpp
Author: rise0chen
Version: 1.0
Date: 2018.4.26
Description: 实时时钟
Usage:
	#include "rtc.hpp"
	RTC_Config(RTC_Time);//初始化RTC
	RTC_SetClock(RTC_Time);//设置RTC时间
	RTC_GetTime();//更新RTC_Time
History: 
	rise0chen   2018.4.26   编写注释
*************************************************/
#include "rtc.h"

/* 月份   1  2  3  4  5  6  7  8  9  10 11 12 */
/* 闰年   31 29 31 30 31 30 31 31 30 31 30 31 */
/* 非闰年 31 28 31 30 31 30 31 31 30 31 30 31 */
const uint8_t RtcLeapMonth[12]   = {31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
const uint8_t RtcCommonMonth[12] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
/* 定义一个全局变量保存时钟 */
RTC_TimeTypeDef RTC_Time;
/* 声明内部函数 */
static uint8_t RTC_CheckLeapYear(uint16_t year);
static uint8_t RTC_SetTime(RTC_TimeTypeDef *time);

void RTC_SetClock(RTC_TimeTypeDef *time){
	while(!(RTC->CRL & (1 << 5)));//等待RTC寄存器操作完成
	RTC->CRL |= 1 << 4;//允许配置
	RTC_SetTime(time);//设置时间
	RTC->CRL &= ~(1 << 4);//配置更新
	while(!(RTC->CRL & (1 << 5)));//等待RTC寄存器操作完成
	RTC_GetTime();//更新时间
}

/****************************************************************************
* Function Name  : RTC_Config
* Description    : 初始化时钟，并初始化内部的时钟信息
* Input          : time：要初始化的时钟
* Output         : None
* Return         : 0：初始化成功；0xFF：初始化失败
****************************************************************************/
int8_t RTC_Config(RTC_TimeTypeDef *time){
	uint32_t timeCount;
	uint32_t i;

	if(BKP->DR1 != 0x5050){
		RCC->APB1ENR |= 0x10000000 | 0x08000000;//使能PWR电源时钟和BKP备份区域外设时钟
		PWR->CR   |= 1 << 8;//取消备份区写保护
		RCC->BDCR |= 1 << 16;//备份区域软复位
		RCC->BDCR &= ~(1 << 16);//备份区域软复位结束
		RCC->BDCR |= 1;//开启外部低速振荡器
		while (!(RCC->BDCR & 0x02)){//等待低速晶振（LSE）就绪
			timeCount++;
			for(i=0; i<0xFF; i++);//延时一下
			/* 外部晶振错误，返回设置失败 */
			if(timeCount > 0x00FFFFF){
				return 0xFF;
			}
		}
		RCC->BDCR |= 1 << 8;//RTC时钟(1<<8:LSE;  1<<9:LSI)
		RCC->BDCR |= 1 << 15;//RTC时钟使能
		while(!(RTC->CRL & (1 << 5)));//等待RTC寄存器操作完成
		while(!(RTC->CRL & (1 << 3)));//等待RTC寄存器同步
		RTC->CRH |= 0x01;  //允许秒中断
		while(!(RTC->CRL & (1 << 5)));//等待RTC寄存器操作完成
		RTC->CRL |= 1 << 4;//允许配置
		RTC->PRLH = 0x0000;
		RTC->PRLL = 32767;//时钟周期设置,理论值为外部低速时钟频率:32767
		RTC_SetTime(time);//设置时间
		RTC->CRL &= ~(1 << 4);//配置更新
		while(!(RTC->CRL & (1 << 5)));//等待RTC寄存器操作完成
		BKP->DR1 = 0x5050;
	}else{
		while(!(RTC->CRL & (1 << 3)));//等待RTC寄存器同步
		RTC->CRH |= 0x01;//允许秒中断
		while(!(RTC->CRL & (1 << 5)));//等待RTC寄存器操作完成
	}
	nvic_config(RTC_IRQn,3,3);//RCT中断分组设置，开启中断
	RTC_GetTime();//更新时间

	return 0;
}

static uint8_t RTC_CheckLeapYear(uint16_t year)
{
	/* 闰年有两种计算方式，第一种是能被四整除且不能被100整除， */
	/* 第二种是能被100整除且能被400整除 */
	if((year % 100) == 0){//能被100整除的，且能被400整除是闰年
		if((year % 400) == 0){
			return 0;//是闰年
		}else{
			return 0xFF;//是平年
		}
	}else{//不能被100整除，但能被4整除是闰年
		if((year % 4) == 0){
			return 0;//是闰年
		}else{
			return 0xFF;//是平年
		}
	}
}

static uint8_t RTC_SetTime(RTC_TimeTypeDef *time){
	uint8_t  leapYear = 0;
	uint16_t i;
	uint32_t secondCount = 0;

	if((time->year < RTC_BASE_YEAR) || (time->year > (RTC_BASE_YEAR+100))){//从2000年到2100年，一共100年
		return 0xFF;//超过时限返回失败
	}
	/* 将所有的年份秒数相加 */
	for(i = RTC_BASE_YEAR; i<time->year; i++){
		if(RTC_CheckLeapYear(i) == 0){//如果年份是闰年
			secondCount += RTC_LEEP_YEAR_SECOND;
		}else{
			secondCount += RTC_COMMON_YEAR_SECOND;
		}
	}
	/* 检测写入年份是闰年还是平年 */
	if(RTC_CheckLeapYear(time->year) == 0){//如果是闰年
		leapYear = 1;//标记为闰年
	}else{
		leapYear = 0;//标记为平年
	}
	/* 所有月份秒数相加 */
	for(i=1; i<time->month; i++){
		if(leapYear == 1){
			secondCount += RtcLeapMonth[i - 1] * RTC_DAY_SECOND;
		}else{
			secondCount += RtcCommonMonth[i - 1] * RTC_DAY_SECOND;
		}
	}
	/* 所有的日期秒数相加 */
	for(i=1; i<time->day; i++){
		secondCount += RTC_DAY_SECOND;
	}
	secondCount += RTC_HOUR_SECOND * time->hour;//小时的秒数
	secondCount += 60 * time->minit;//分钟的秒数
	secondCount += time->second;//加上秒数
	RCC->APB1ENR |= 0x10000000 | 0x08000000;//使能PWR电源时钟和BKP备份区域外设时钟
	PWR->CR |= 1 << 8;//取消备份区写保护
	/* 写入更新时间 */
	RTC->CRL |= 1 << 4;//允许配置
	RTC->CNTL = secondCount & 0xFFFF;
	RTC->CNTH = secondCount >> 16;
	RTC->CRL &= ~(1 << 4);//配置更新
	while(!(RTC->CRL & (1 << 5)));//等待RTC寄存器操作完成

	return 0;//设置成功返回0
}

void RTC_GetTime(void){
	uint8_t leapYear = 0, i = 0;
	uint32_t secondCount = 0;
	uint32_t day;

	/* 读取时钟计数器的值 */
	secondCount =  RTC->CNTH;
	secondCount <<= 16;
	secondCount |= RTC->CNTL;

	day = secondCount / RTC_DAY_SECOND;//求出天数
	secondCount = secondCount % RTC_DAY_SECOND;//求出剩余秒数

	RTC_Time.year = RTC_BASE_YEAR;
	RTC_Time.week = (day + 6) % 7;//因为2000年1月1日是星期六所以加6
	while(day >= 365){
		if(RTC_CheckLeapYear(RTC_Time.year) == 0){//是闰年
			day -= 366;//闰年有366天
		}else{
			day -= 365;//平年有365天
		}
		RTC_Time.year++;
	}
	if(RTC_CheckLeapYear(RTC_Time.year) == 0){
			leapYear = 1;//如果是闰年标记
	}
	i = 0;
	RTC_Time.month = 1;
	while(day >= 28){
		if(leapYear == 1){
			if(day < RtcCommonMonth[i]){//天数不够一个月
				break;
			}
			day -= RtcLeapMonth[i];//减去闰年该月的天数
		}else{
			if(day < RtcCommonMonth[i]){//天数不够一个月
				break;
			}
			day -= RtcCommonMonth[i];//减去平年该月的天数
		}
		RTC_Time.month++;//月份加1
		i++;//月份数组加1
	}
	RTC_Time.day = day + 1;//月份剩下的天数就是日期(日期从1号开始)
	RTC_Time.hour = secondCount / RTC_HOUR_SECOND;//求出小时
	RTC_Time.minit = secondCount % RTC_HOUR_SECOND / 60;//求出分钟
	RTC_Time.second = secondCount % RTC_HOUR_SECOND %60;//求出秒
}

