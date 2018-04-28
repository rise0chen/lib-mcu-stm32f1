#ifndef __DS18B20_H
#define __DS18B20_H
#include "sys.hpp"

//温度传感器
namespace DS18B20{
	extern Gpio *DQ;
	
	void init(void);
	void write(u8 dat);
	u8 read(void);
	u8 getTemp(void);
}

#endif //__DS18B20_H
