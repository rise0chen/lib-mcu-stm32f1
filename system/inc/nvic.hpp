#ifndef __NVIC_H
#define __NVIC_H
#include "sys.hpp"

class Nvic{
	public:
		void setVectorTable(u32 vectTab, u32 offset);//设置偏移地址
		void configGroup(u8 group);//设置NVIC分组
		void config(u8 channel, u8 preemptionPriority, u8 subPriority=3);//设置中断
		void disable(void);//关闭所有中断
		void enable(void);//开启所有中断
	
	private:
		u8 cfgGroup;
};
extern Nvic nvic;

#endif //__NVIC_H
