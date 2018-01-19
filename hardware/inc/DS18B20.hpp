#ifndef __DS18B20_H
#define __DS18B20_H
#include "sys.hpp"

//温度传感器
namespace DS18B20{
	extern gpio &DQ;
	
	u8 GetTemp();
}

#endif //__DS18B20_H
