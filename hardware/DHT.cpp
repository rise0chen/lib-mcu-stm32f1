/*************************************************
Copyright (C), 2018-2028, Crise Tech. Co., Ltd.
File name: DHT.cpp
Author: rise0chen
Version: 1.0
Date: 2019.08.26
Description: DST11/DHT22温湿度传感器
Usage:
        #include "DHT.h"
        DHT dht11(&gpio_default);
        dht11.getData();
        //dht11.temp
        //dht11.humi
History:
        rise0chen   2019.08.26   编写程序
*************************************************/
#include "DHT.h"

/*************************************************
Function: DHT::DHT
Description: DHT
Input: P_DQ  数据引脚
Return: DHT类
*************************************************/
DHT::DHT(Gpio* P_DQ) {
  DQ = P_DQ;
}

/*************************************************
Function: DHT::setGPIO
Description: 设置引脚
Input: P_DQ  数据引脚
Return: void
*************************************************/
void DHT::setGPIO(Gpio* P_DQ) {
  DQ = P_DQ;
}

/*************************************************
Function: DHT::start
Description: 发送起始信号
Input: void
Return: void
*************************************************/
void DHT::start(void) {
  DQ->config(P_PPO);
  *DQ->O = 1;
  delay_us(20);
  *DQ->O = 0;
  delay_ms(20);
  *DQ->O = 1;
  delay_us(40);

  DQ->config(P_FIN);
  while (!*DQ->I)
    ;  //等待80us低电平
  while (*DQ->I)
    ;  //等待80us高电平
}

/*************************************************
Function: DHT::read
Description: 读单字节数据
Input: void
Return: 单字节数据
*************************************************/
uint8_t DHT::read(void) {
  uint8_t i, value = 0;
  DQ->config(P_FIN);
  for (i = 0; i < 8; i++) {
    while (!*DQ->I)
      ;  //等待开始
    delay_us(50);
    value <<= 1;
    if (*DQ->I == 1) {
      value += 1;
    }
    while (*DQ->I)
      ;  //等待结束
  }
  return value;
}

/*************************************************
Function: DHT::getData
Description: 读取温湿度
Input: void
Return: 错误码(0成功)
*************************************************/
int8_t DHT::getData(void) {
  uint8_t humiH, humiL, tempH, tempL, check;
  start();         //读数据起始信号
  humiH = read();  //湿度整数部分
  humiL = read();  //湿度小数部分
  tempH = read();  //温度整数部分
  tempL = read();  //温度小数部分
  check = read();  //和校验
  if ((uint8_t)(humiH + humiL + tempH + tempL) != check) {
    return 1;
  }

  humi[0] = humiH;
  humi[1] = humiL;
  temp[0] = tempH;
  temp[1] = tempL;
  return 0;
}
