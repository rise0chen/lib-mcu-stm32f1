#ifndef __NVIC_H
#define __NVIC_H
#include "sys.hpp"

class Nvic{
	public:
		void setVectorTable(uint32_t vectTab, uint32_t offset);//设置偏移地址
		void configGroup(uint8_t group);//设置NVIC分组
		void config(uint8_t channel, uint8_t preemptionPriority, uint8_t subPriority=3);//设置中断
		void disable(void);//关闭所有中断
		void enable(void);//开启所有中断
	
	private:
		uint8_t cfgGroup;
};
extern Nvic nvic;

#endif //__NVIC_H
