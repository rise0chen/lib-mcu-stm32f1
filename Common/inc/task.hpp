#ifndef __TASK_H
#define __TASK_H
#include "sys.hpp"

extern uint32_t timeSysTick;
extern uint16_t timeOneSysTick;
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
			uint8_t uid;//唯一ID
			Task_Status status;//当前状态
			uint32_t startTime;//开始时间
			uint32_t endTime;//结束时间
			void (*func)(void);//执行函数
			uint16_t times;//执行次数
			uint16_t timesRun;//已执行次数
			uint16_t interval;//执行间隔 0每次main函数都执行
		}Task_TypeDef;//任务结构体
		Task_TypeDef* taskType[TASK_MAXNUM];
		
		void init(uint16_t nms);
		void add(uint8_t uid, void (*func)(void), uint16_t in=1, uint16_t ts=0xFFFF, uint16_t st=0, uint16_t et=0xFFFF);
		void update(uint8_t uid, void (*func)(void), uint16_t in=1, uint16_t ts=0xFFFF, uint16_t st=0, uint16_t et=0xFFFF);
		void cmd(uint8_t uid, Task_Status status);
		void run(void);
		
	private:
};
extern Task task;

#endif //__TASK_H
