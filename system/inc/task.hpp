#ifndef __TASK_H
#define __TASK_H
#include "sys.hpp"

extern u32 RunTime;

namespace task{
	#define TASK_READY 0x64
	#define TASK_FINISH 0x65
	typedef struct{
		u8 uid;//唯一ID
		u8 status;//当前状态
		u32 startTime;//开始时间
		u32 endTime;//结束时间
		void (*func)(void);//执行函数
		u8 times;//执行次数 0无数次
		u8 timesRun;//已执行次数
		u8 interval;//执行间隔 0每次main函数都执行
	}task_TypeDef;//任务结构体
	
	void init(void);
	void add(u8 uid, void (*func)(void), u8 st=0, u8 et=0xff, u8 in=1, u8 ts=0);
	void update(u8 uid, void (*func)(void), u8 st=0, u8 et=0xff, u8 in=1, u8 ts=0);
	void run(void);
}

#endif //__TASK_H
