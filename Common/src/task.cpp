/*************************************************
Copyright (C), 2018-2028, Crise Tech. Co., Ltd.
File name: Can.cpp
Author: rise0chen
Version: 1.0
Date: 2018.4.26
Description: Task定时任务 类
Usage:
	#include "Task.hpp"
	task.init(); //依赖SysTick
	task.add(0x01, myTest, 0, 0xff, 10, 0); //新建任务
	task.update(0x01, myTest, 0, 0xff, 10, 0); //更新任务
	task.run(); //执行任务(必须放在loop大循环里)
History: 
	rise0chen   2018.4.26   改为Class; 编写注释
*************************************************/
#include "Task.hpp"

Task task;
#define fac_us (9)//us延时倍乘数 (sysclk/8 000 000)
#define fac_ms (fac_us*1000)//ms延时倍乘数
u32 RunTime;//单位秒


/*************************************************
Function: Can::init
Description: 初始化SysTick心跳定时器
Calls: 
Called By: 
Input: void
Return: void
*************************************************/
void Task::init(void){
	SysTick->CTRL = 0;//复位
	SysTick->VAL  = 0x00;//清空
	SysTick->LOAD = 1000*fac_ms;//1s心跳
	SysTick->CTRL = 3;//8分频 中断 使能
}

/*************************************************
Function: Can::add
Description: 新建任务
Calls: 
Called By: 
Input: 
	uid  任务编码 0~TASK_MAXNUM
	func 需要执行的函数
	st   几秒后开始执行 0立即执行
	et   几秒后结束执行 0永不执行 0xff永不停止
	in   几秒执行一次   0每次loop循环都执行
	ts   最多执行几次   0永不执行 0xff无限次
Return: void
*************************************************/
void Task::add(u8 uid, void (*func)(void), u8 st, u8 et, u8 in, u8 ts){
	taskType[uid]=new Task_TypeDef;
	taskType[uid]->uid=uid;
	taskType[uid]->status = FINISH;
	taskType[uid]->startTime=RunTime+st;
	if(et==0){
		taskType[uid]->endTime=0;
	}else if(et==0xff){
		taskType[uid]->endTime=0xffffffff;
	}else{
		taskType[uid]->endTime=RunTime+et;
	}
	taskType[uid]->func=func;
	taskType[uid]->times=ts;
	taskType[uid]->timesRun=0;
	taskType[uid]->interval=in;
}

/*************************************************
Function: Can::update
Description: 更新任务
Calls: 
Called By: 
Input: 
	uid  任务编码 0~TASK_MAXNUM
	func 需要执行的函数
	st   几秒后开始执行 0立即执行
	et   几秒后结束执行 0永不执行 0xff永不停止
	in   几秒执行一次   0每次loop循环都执行
	ts   最多执行几次   0永不执行 0xff无限次
Return: void
*************************************************/
void Task::update(u8 uid, void (*func)(void), u8 st, u8 et, u8 in, u8 ts){
	taskType[uid]->status = FINISH;
	taskType[uid]->startTime=RunTime+st;
	if(et==0){
		taskType[uid]->endTime=0;
	}else if(et==0xff){
		taskType[uid]->endTime=0xffffffff;
	}else{
		taskType[uid]->endTime=RunTime+et;
	}
	taskType[uid]->func=func;
	taskType[uid]->times=ts;
	taskType[uid]->timesRun=0;
	taskType[uid]->interval=in;
}

/*************************************************
Function: Can::run
Description: 执行任务
Calls: 
Called By: 
Input: void
Return: void
*************************************************/
void Task::run(void){
	for(u16 i=0; i<TASK_MAXNUM; i++){
		if(taskType[i]->status == READY){
			if(taskType[i]->interval == 0){
				if(RunTime>=taskType[i]->startTime && RunTime<=taskType[i]->endTime){
					if(taskType[i]->times==0xff || taskType[i]->timesRun < taskType[i]->times){
						taskType[i]->timesRun++;
						taskType[i]->func();
					}
				}
			}else{
				taskType[i]->timesRun++;
				taskType[i]->func();
				taskType[i]->status = FINISH;
			}
		}
	}
}

/*************************************************
Function: SysTick_Handler
Description: 心跳定时器中断(每1s触发)
Calls: 
Called By: 
Input: void
Return: void
*************************************************/
_C void SysTick_Handler(void){
	RunTime++;
	for(u16 i=0; i<TASK_MAXNUM; i++){
		if(task.taskType[i]->status == task.FINISH){
			if(RunTime>=task.taskType[i]->startTime && RunTime<=task.taskType[i]->endTime){
				if(task.taskType[i]->times==0xff || task.taskType[i]->timesRun < task.taskType[i]->times){
					if(task.taskType[i]->interval == 0){
						task.taskType[i]->status = task.READY;
					}else if((RunTime-task.taskType[i]->startTime)%task.taskType[i]->interval==0){
						task.taskType[i]->status = task.READY;
					}
				}
			}
		}
	}
}
