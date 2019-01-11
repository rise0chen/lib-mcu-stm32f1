/*************************************************
Copyright (C), 2018-2028, Crise Tech. Co., Ltd.
File name: Mpu6050.cpp
Author: rise0chen
Version: 1.0
Date: 2018.4.26
Description: Mpu6050陀螺仪芯片
Usage:
	#include "Mpu6050.hpp"
	Mpu6050 mpu6050(&i2c2, 0x68);
	mpu6050.init();
	mpu6050.getGYRO();
	mpu6050.getACCEL();
	usart1.printf("GYRO_X:%d;\r\n",mpu6050_GYRO[0]);
	usart1.printf("GYRO_Y:%d;\r\n",mpu6050_GYRO[1]);
	usart1.printf("GYRO_Z:%d;\r\n",mpu6050_GYRO[2]);
	usart1.printf("ACCEL_X:%d;\r\n",mpu6050_ACCEL[0]);
	usart1.printf("ACCEL_Y:%d;\r\n",mpu6050_ACCEL[1]);
	usart1.printf("ACCEL_Z:%d;\r\n",mpu6050_ACCEL[2]);
History: 
	rise0chen   2018.4.26   编写注释
*************************************************/
#include "Mpu6050.h"

/*************************************************
Function: mpu6050_Mpu6050
Description: Mpu6050构造函数
Input: 
	i2c_com  I2C外设
	i2c_self->addr I2C地址
Return: DS18B20类
*************************************************/
Mpu6050Struct* mpu6050_new(I2cStruct* i2c_com, uint8_t i2c_addr){
  Mpu6050Struct* self = (Mpu6050Struct*)malloc(sizeof(Mpu6050Struct));
	self->com  = i2c_com;
	self->addr = i2c_addr;
  return self;
}

/*************************************************
Function: mpu6050_init
Description: 初始化Mpu6050
Input: void
Return: void
*************************************************/
void mpu6050_init(Mpu6050Struct* self){
	mpu6050_writeByte(self, MPU_PWR_MGMT1_REG,0X80);//复位Mpu6050
	delay_ms(100);
	mpu6050_writeByte(self, MPU_PWR_MGMT1_REG,0X00);//唤醒Mpu6050
	mpu6050_writeByte(self, MPU_GYRO_CFG_REG,3<<3);//陀螺仪满量程范围 ±2000dps  16.384=1°/s
	mpu6050_writeByte(self, MPU_ACCEL_CFG_REG,0<<3);//加速度满量程范围 ±2g  16384=1g
	mpu6050_writeByte(self, MPU_USER_CTRL_REG,0X00);//I2C主模式关闭
	mpu6050_writeByte(self, MPU_FIFO_EN_REG,0X00);//关闭FIFO
	if(mpu6050_readByte(self, MPU_DEVICE_ID_REG)==self->addr){//器件ID正确
		mpu6050_writeByte(self, MPU_PWR_MGMT1_REG,0X09);//禁用温度传感器,PLL X轴为参考
		mpu6050_writeByte(self, MPU_PWR_MGMT2_REG,0X00);//加速度与陀螺仪都工作
		mpu6050_writeByte(self, MPU_SAMPLE_RATE_REG,(1000/50-1));//采样率 50Hz
		mpu6050_writeByte(self, MPU_CFG_REG,4);//数字低通滤波器 25Hz
		mpu6050_writeByte(self, MPU_INT_EN_REG,0X40);//开启运动中断
		mpu6050_writeByte(self, MPU_INTBP_CFG_REG,0XF0);//INT引脚开漏输出，中断时低电平
		mpu6050_writeByte(self, MPU_MOTION_DET_REG,0x01);//运动阈值
		mpu6050_writeByte(self, MPU_MOTION_DUR_REG,0x14);//计数器阈值
		mpu6050_writeByte(self, MPU_MDETECT_CTRL_REG,0xC8);
	}
}

/*************************************************
Function: mpu6050_getGYRO
Description: 读取倾斜角,输出到GYRO[]
Input: void
Return: void
*************************************************/
void mpu6050_getGYRO(Mpu6050Struct* self){
	uint8_t buf[2];
	buf[0]=mpu6050_readByte(self, MPU_GYRO_XOUTH_REG);
	buf[1]=mpu6050_readByte(self, MPU_GYRO_XOUTL_REG);
	self->GYRO[0]= (buf[0]<<8)|buf[1];
	buf[0]=mpu6050_readByte(self, MPU_GYRO_YOUTH_REG);
	buf[1]=mpu6050_readByte(self, MPU_GYRO_YOUTL_REG);
	self->GYRO[1]= (buf[0]<<8)|buf[1];
	buf[0]=mpu6050_readByte(self, MPU_GYRO_ZOUTH_REG);
	buf[1]=mpu6050_readByte(self, MPU_GYRO_ZOUTL_REG);
	self->GYRO[2]= (buf[0]<<8)|buf[1];
}

/*************************************************
Function: mpu6050_getACCEL
Description: 读取加速度,输出到ACCEL[]
Input: void
Return: void
*************************************************/
void mpu6050_getACCEL(Mpu6050Struct* self){
	uint8_t buf[2];
	buf[0]=mpu6050_readByte(self, MPU_ACCEL_XOUTH_REG);
	buf[1]=mpu6050_readByte(self, MPU_ACCEL_XOUTL_REG);
	self->ACCEL[0]= (buf[0]<<8)|buf[1];
	buf[0]=mpu6050_readByte(self, MPU_ACCEL_YOUTH_REG);
	buf[1]=mpu6050_readByte(self, MPU_ACCEL_YOUTL_REG);
	self->ACCEL[1]= (buf[0]<<8)|buf[1];
	buf[0]=mpu6050_readByte(self, MPU_ACCEL_ZOUTH_REG);
	buf[1]=mpu6050_readByte(self, MPU_ACCEL_ZOUTL_REG);
	self->ACCEL[2]= (buf[0]<<8)|buf[1];
}

/*************************************************
Function: mpu6050_writeByte
Description: 向寄存器写单字节
Input: 
	reg   寄存器地址
	data  单字节数据
Return: void
*************************************************/
void mpu6050_writeByte(Mpu6050Struct* self, uint8_t reg,uint8_t data){
	i2c_start(self->com);
	i2c_write(self->com, (self->addr<<1)|0);//发送器件地址+写命令
	i2c_write(self->com, reg);//写寄存器地址
	i2c_write(self->com, data);//发送数据
	i2c_stop(self->com);
}

/*************************************************
Function: mpu6050_readByte
Description: 从寄存器读单字节
Input: 
	reg   寄存器地址
Return: 单字节数据
*************************************************/
uint8_t mpu6050_readByte(Mpu6050Struct* self, uint8_t reg){
	uint8_t res;
	i2c_start(self->com); 
	i2c_write(self->com, (self->addr<<1)|0);//发送器件地址+写命令
	i2c_write(self->com, reg);//写寄存器地址
	i2c_start(self->com);
	i2c_write(self->com, (self->addr<<1)|1);//发送器件地址+读命令
	res=i2c_read(self->com, 0);//读取数据,发送nACK
	i2c_stop(self->com);//产生一个停止条件
	return res;
}
