/*************************************************
Copyright (C), 2018-2028, Crise Tech. Co., Ltd.
File name: Mpu6050.cpp
Author: rise0chen
Version: 1.0
Date: 2018.4.26
Description: Mpu6050陀螺仪芯片
Usage:
	#include "Mpu6050.hpp"
	mpu6050.init();
	mpu6050.getGYRO();
	mpu6050.getACCEL();
	usart1.printf("GYRO_X:%d;\r\n",Mpu6050::GYRO[0]);
	usart1.printf("GYRO_Y:%d;\r\n",Mpu6050::GYRO[1]);
	usart1.printf("GYRO_Z:%d;\r\n",Mpu6050::GYRO[2]);
	usart1.printf("ACCEL_X:%d;\r\n",Mpu6050::ACCEL[0]);
	usart1.printf("ACCEL_Y:%d;\r\n",Mpu6050::ACCEL[1]);
	usart1.printf("ACCEL_Z:%d;\r\n",Mpu6050::ACCEL[2]);
History: 
	rise0chen   2018.4.26   编写注释
*************************************************/
#include "Mpu6050.hpp"

Mpu6050 mpu6050(&i2c2, 0x68);

Mpu6050::Mpu6050(I2c *com, u8 addr):com(com),addr(addr){
	
}

/*************************************************
Function: Mpu6050::init
Description: 初始化Mpu6050
Input: void
Return: void
*************************************************/
void Mpu6050::init(void){
	writeByte(MPU_PWR_MGMT1_REG,0X80);//复位Mpu6050
	delay_ms(100);
	writeByte(MPU_PWR_MGMT1_REG,0X00);//唤醒Mpu6050
	writeByte(MPU_GYRO_CFG_REG,3<<3);//陀螺仪满量程范围 ±2000dps  16.384=1°/s
	writeByte(MPU_ACCEL_CFG_REG,0<<3);//加速度满量程范围 ±2g  16384=1g
	writeByte(MPU_USER_CTRL_REG,0X00);//I2C主模式关闭
	writeByte(MPU_FIFO_EN_REG,0X00);//关闭FIFO
	if(readByte(MPU_DEVICE_ID_REG)==addr){//器件ID正确
		writeByte(MPU_PWR_MGMT1_REG,0X09);//禁用温度传感器,PLL X轴为参考
		writeByte(MPU_PWR_MGMT2_REG,0X00);//加速度与陀螺仪都工作
		writeByte(MPU_SAMPLE_RATE_REG,(1000/50-1));//采样率 50Hz
		writeByte(MPU_CFG_REG,4);//数字低通滤波器 25Hz
		writeByte(MPU_INT_EN_REG,0X40);//开启运动中断
		writeByte(MPU_INTBP_CFG_REG,0XF0);//INT引脚开漏输出，中断时低电平
		writeByte(MPU_MOTION_DET_REG,0x01);//运动阈值
		writeByte(MPU_MOTION_DUR_REG,0x14);//计数器阈值
		writeByte(MPU_MDETECT_CTRL_REG,0xC8);
	}
}

/*************************************************
Function: Mpu6050::getGYRO
Description: 读取倾斜角,输出到GYRO[]
Input: void
Return: void
*************************************************/
void Mpu6050::getGYRO(void){
	u8 buf[2];
	buf[0]=readByte(MPU_GYRO_XOUTH_REG);
	buf[1]=readByte(MPU_GYRO_XOUTL_REG);
	GYRO[0]= (buf[0]<<8)|buf[1];
	buf[0]=readByte(MPU_GYRO_YOUTH_REG);
	buf[1]=readByte(MPU_GYRO_YOUTL_REG);
	GYRO[1]= (buf[0]<<8)|buf[1];
	buf[0]=readByte(MPU_GYRO_ZOUTH_REG);
	buf[1]=readByte(MPU_GYRO_ZOUTL_REG);
	GYRO[2]= (buf[0]<<8)|buf[1];
}

/*************************************************
Function: Mpu6050::getACCEL
Description: 读取加速度,输出到ACCEL[]
Input: void
Return: void
*************************************************/
void Mpu6050::getACCEL(void){
	u8 buf[2];
	buf[0]=readByte(MPU_ACCEL_XOUTH_REG);
	buf[1]=readByte(MPU_ACCEL_XOUTL_REG);
	ACCEL[0]= (buf[0]<<8)|buf[1];
	buf[0]=readByte(MPU_ACCEL_YOUTH_REG);
	buf[1]=readByte(MPU_ACCEL_YOUTL_REG);
	ACCEL[1]= (buf[0]<<8)|buf[1];
	buf[0]=readByte(MPU_ACCEL_ZOUTH_REG);
	buf[1]=readByte(MPU_ACCEL_ZOUTL_REG);
	ACCEL[2]= (buf[0]<<8)|buf[1];
}

/*************************************************
Function: Mpu6050::writeByte
Description: 向寄存器写单字节
Input: 
	reg   寄存器地址
	data  单字节数据
Return: void
*************************************************/
void Mpu6050::writeByte(u8 reg,u8 data){
	com->start();
	com->write((addr<<1)|0);//发送器件地址+写命令
	com->write(reg);//写寄存器地址
	com->write(data);//发送数据
	com->stop();
}

/*************************************************
Function: Mpu6050::readByte
Description: 从寄存器读单字节
Input: 
	reg   寄存器地址
Return: 单字节数据
*************************************************/
u8 Mpu6050::readByte(u8 reg){
	u8 res;
	com->start(); 
	com->write((addr<<1)|0);//发送器件地址+写命令
	com->write(reg);//写寄存器地址
	com->start();
	com->write((addr<<1)|1);//发送器件地址+读命令
	res=com->read(0);//读取数据,发送nACK
	com->stop();//产生一个停止条件
	return res;
}
