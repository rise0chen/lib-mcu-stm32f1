#ifndef __TASK_H
#define __TASK_H
#include "sys.hpp"

extern u32 timeSysTick;
extern u16 timeOneSysTick;
#define timeSysRun timeSysTick*timeOneSysTick  //系统运行时间(ms) 最大49天

#define TASK_MAXNUM 128  //最大任务数(uid从0到TASK_MAXNUM)(不得超过255)
class Task{
	public:
		typedef enum {
			READY  = 0x61,//准备执行
			RUN    = 0x66,//正在执行
			FINISH = 0x6F,//执行完成
			STOP   = 0xF0,//停止执行
			ERROR  = 0xFF,//未知错误
		} Task_Status;//状态码
		typedef struct{
			u8 uid;//唯一ID
			Task_Status status;//当前状态
			u32 startTime;//开始时间
			u32 endTime;//结束时间
			void (*func)(void);//执行函数
			u16 times;//执行次数
			u16 timesRun;//已执行次数
			u16 interval;//执行间隔 0每次main函数都执行
		}Task_TypeDef;//任务结构体
		Task_TypeDef* taskType[TASK_MAXNUM];
		
		void init(u16 nms);
		void add(u8 uid, void (*func)(void), u16 in=1, u16 ts=0xFFFF, u16 st=0, u16 et=0xFFFF);
		void update(u8 uid, void (*func)(void), u16 in=1, u16 ts=0xFFFF, u16 st=0, u16 et=0xFFFF);
		void cmd(u8 uid, Task_Status status);
		void run(void);
		
	private:
};
extern Task task;

#endif //__TASK_H
