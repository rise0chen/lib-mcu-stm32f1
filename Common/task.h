#ifndef __TASK_H
#define __TASK_H
#include "sys.h"

extern uint32_t timeSysTick;
extern uint16_t timeOneSysTick;
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
	uint8_t uid;//唯一ID
	TaskStatusEnum status;//运行状态
	uint32_t startTime;//开始时间
	uint32_t endTime;//结束时间
	void (*func)(void);//执行函数
	uint16_t times;//执行次数
	uint16_t timesRun;//已执行次数
	uint16_t interval;//执行间隔 0每次main函数都执行
}TaskStruct;//任务结构体

void task_init(uint16_t nms);
void task_add(uint8_t uid, void (*func)(void), uint16_t in, uint16_t ts, uint16_t st, uint16_t et);
void task_update(uint8_t uid, void (*func)(void), uint16_t in, uint16_t ts, uint16_t st, uint16_t et);
void task_cmd(uint8_t uid, TaskStatusEnum status);
void task_run(void);


#endif //__TASK_H
