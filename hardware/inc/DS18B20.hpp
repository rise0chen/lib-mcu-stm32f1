#ifndef __DS18B20_H
#define __DS18B20_H
#include "sys.hpp"

//温度传感器
class DS18B20{
	public:
		u8 getTemp(Gpio *P_DQ);
	
	private:
		Gpio *DQ = &gpio_default;
	
		void init(Gpio *P_DQ);
		void write(u8 dat);
		u8 read(void);
};
extern DS18B20 ds18b20;

#endif //__DS18B20_H
