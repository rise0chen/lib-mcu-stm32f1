#ifndef __NVIC_H
#define __NVIC_H
#include "sys.hpp"

namespace nvic{
	void SetVectorTable(u32 VectTab, u32 Offset);//设置偏移地址
	void GroupConfig(u8 Group);//设置NVIC分组
	void Init(u8 Channel, u8 PreemptionPriority, u8 SubPriority=3);//设置中断

	void DISABLE(void);//关闭所有中断
	void ENABLE(void);//开启所有中断
}
#endif //__NVIC_H
