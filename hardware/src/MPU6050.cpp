#include "MPU6050.hpp"

namespace MPU6050{
	i2c *com = &i2c2;
	u8 deviceAddr = 0x68;//AD0默认接GND
	short GYRO[3],ACCEL[3];
	
	void Init(void){
		Write_Byte(MPU_PWR_MGMT1_REG,0X80);//复位MPU6050
		delay_ms(100);
		Write_Byte(MPU_PWR_MGMT1_REG,0X00);//唤醒MPU6050
		Write_Byte(MPU_GYRO_CFG_REG,3<<3);//陀螺仪满量程范围 ±2000dps  16.384=1°/s
		Write_Byte(MPU_ACCEL_CFG_REG,0<<3);//加速度满量程范围 ±2g  16384=1g
		Write_Byte(MPU_USER_CTRL_REG,0X00);//I2C主模式关闭
		Write_Byte(MPU_FIFO_EN_REG,0X00);//关闭FIFO
		if(Read_Byte(MPU_DEVICE_ID_REG)==deviceAddr){//器件ID正确
			Write_Byte(MPU_PWR_MGMT1_REG,0X09);//禁用温度传感器,PLL X轴为参考
			Write_Byte(MPU_PWR_MGMT2_REG,0X00);//加速度与陀螺仪都工作
			Write_Byte(MPU_SAMPLE_RATE_REG,(1000/50-1));//采样率 50Hz
			Write_Byte(MPU_CFG_REG,4);//数字低通滤波器 25Hz
			Write_Byte(MPU_INT_EN_REG,0X40);//开启运动中断
			Write_Byte(MPU_INTBP_CFG_REG,0XF0);//INT引脚开漏输出，中断时低电平
			Write_Byte(MPU_MOTION_DET_REG,0x01);//运动阈值
			Write_Byte(MPU_MOTION_DUR_REG,0x14);//计数器阈值
			Write_Byte(MPU_MDETECT_CTRL_REG,0xC8);
		}
	}

	void Get_GYRO(void){
		u8 BUF[2];
		BUF[0]=Read_Byte(MPU_GYRO_XOUTH_REG);
		BUF[1]=Read_Byte(MPU_GYRO_XOUTL_REG);
		GYRO[0]= (BUF[0]<<8)|BUF[1];
		BUF[0]=Read_Byte(MPU_GYRO_YOUTH_REG);
		BUF[1]=Read_Byte(MPU_GYRO_YOUTL_REG);
		GYRO[1]= (BUF[0]<<8)|BUF[1];
		BUF[0]=Read_Byte(MPU_GYRO_ZOUTH_REG);
		BUF[1]=Read_Byte(MPU_GYRO_ZOUTL_REG);
		GYRO[2]= (BUF[0]<<8)|BUF[1];
	}
	void Get_ACCEL(void){
		u8 BUF[2];
		BUF[0]=Read_Byte(MPU_ACCEL_XOUTH_REG);
		BUF[1]=Read_Byte(MPU_ACCEL_XOUTL_REG);
		ACCEL[0]= (BUF[0]<<8)|BUF[1];
		BUF[0]=Read_Byte(MPU_ACCEL_YOUTH_REG);
		BUF[1]=Read_Byte(MPU_ACCEL_YOUTL_REG);
		ACCEL[1]= (BUF[0]<<8)|BUF[1];
		BUF[0]=Read_Byte(MPU_ACCEL_ZOUTH_REG);
		BUF[1]=Read_Byte(MPU_ACCEL_ZOUTL_REG);
		ACCEL[2]= (BUF[0]<<8)|BUF[1];
	}
	
	void Write_Byte(u8 reg,u8 data){
		com.Start(); 
		com.Write((deviceAddr<<1)|0);//发送器件地址+写命令
		com.Write(reg);//写寄存器地址
		com.Write(data);//发送数据
		com.Stop();
	}
	u8 Read_Byte(u8 reg){
		u8 res;
		com.Start(); 
		com.Write((deviceAddr<<1)|0);//发送器件地址+写命令
		com.Write(reg);//写寄存器地址
		com.Start();
		com.Write((deviceAddr<<1)|1);//发送器件地址+读命令
		res=com.Read(0);//读取数据,发送nACK
		com.Stop();//产生一个停止条件
		return res;
	}
}
