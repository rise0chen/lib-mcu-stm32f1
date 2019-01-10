#ifndef __TASK_H
#define __TASK_H
#include "sys.h"

extern u32 timeSysTick;
extern u16 timeOneSysTick;
#define timeSysRun timeSysTick*timeOneSysTick  //系统运行时间(ms) 最大49天

#define TASK_NUM_MAX 128  //最大任务数(uid从0到TASK_MAXNUM)(不得超过255)

typedef enum {
	taskStatus_ready  = 0x61,//准备执行
	taskStatus_run    = 0x66,//正在执行
	taskStatus_finish = 0x6F,//执行完成
	taskStatus_stop   = 0xF0,//停止执行
	taskStatus_error  = 0xFF,//未知错误
} TaskStatusEnum;//状态码
typedef struct{
	u8 uid;//唯一ID
	TaskStatusEnum status;//运行状态
	u32 startTime;//开始时间
	u32 endTime;//结束时间
	void (*func)(void);//执行函数
	u16 times;//执行次数
	u16 timesRun;//已执行次数
	u16 interval;//执行间隔 0每次main函数都执行
}TaskStruct;//任务结构体

void task_init(u16 nms);
void task_add(u8 uid, void (*func)(void), u16 in, u16 ts, u16 st, u16 et);
void task_update(u8 uid, void (*func)(void), u16 in, u16 ts, u16 st, u16 et);
void task_cmd(u8 uid, TaskStatusEnum status);
void task_run(void);


#endif //__TASK_H
