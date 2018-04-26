#ifndef __PWR_H
#define __PWR_H
#include "sys.hpp"

namespace pwr{
	inline void reset(void);  //系统软复位
	void sleep(u8 next=0);//睡眠模式
	void stop(void);//停止模式
	void standby(void);//待机模式
}

#endif //__PWR_H
