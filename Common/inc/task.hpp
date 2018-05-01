#ifndef __TASK_H
#define __TASK_H
#include "sys.hpp"

extern u32 RunTime;

#define TASK_MAXNUM 128  //最大任务数(uid从0到TASK_MAXNUM)(不得超过255)
class Task{
	public:
		typedef enum {
			READY  = 0x64,//准备执行
			FINISH = 0x65,//执行完成
			ERROR  = 0xFF,//未知错误
		} Task_Status;//状态码
		typedef struct{
			u8 uid;//唯一ID
			Task_Status status;//当前状态
			u32 startTime;//开始时间
			u32 endTime;//结束时间
			void (*func)(void);//执行函数
			u8 times;//执行次数 0无数次
			u8 timesRun;//已执行次数
			u8 interval;//执行间隔 0每次main函数都执行
		}Task_TypeDef;//任务结构体
		Task_TypeDef* taskType[TASK_MAXNUM];
		
		void init(void);
		void add(u8 uid, void (*func)(void), u8 in=1, u8 ts=0xff, u8 st=0, u8 et=0xff);
		void update(u8 uid, void (*func)(void), u8 in=1, u8 ts=0xff, u8 st=0, u8 et=0xff);
		void run(void);
		
	private:
};
extern Task task;

#endif //__TASK_H
