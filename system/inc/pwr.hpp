#ifndef __PWR_H
#define __PWR_H
#include "sys.hpp"

namespace pwr{
	inline void Reset(void);  //系统软复位
	void Sleep(u8 next=0);//睡眠模式
	void Stop(void);//停止模式
	void Standby(void);//待机模式
}

#endif //__PWR_H
