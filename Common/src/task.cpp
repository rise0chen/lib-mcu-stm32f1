/*************************************************
Copyright (C), 2018-2028, Crise Tech. Co., Ltd.
File name: Can.cpp
Author: rise0chen
Version: 1.0
Date: 2018.4.26
Description: Task定时任务 类
Usage:
	#include "Task.hpp"
	task.init(100); //依赖SysTick
	task.add(0x01, myTest, 10000/timeOneSysTick, 0xff); //新建任务 10000ms执行1次
	task.update(0x01, myTest, 10000/timeOneSysTick, 0xff, 0, 0xff); //更新任务
	task.run(); //执行任务(必须放在loop大循环里)
History: 
	rise0chen   2018.4.26   改为Class; 编写注释
*************************************************/
#include "Task.hpp"

Task task;
#define fac_us (9)  //us倍乘数 (rcc.clkSys/8 000 000)
#define fac_ms (fac_us*1000)  //ms倍乘数

u16 timeOneSysTick = 0;//1次系统心跳的时间(ms)
u32 timeSysTick = 0;//系统心跳次数

/*************************************************
Function: Task::init
Description: 初始化SysTick心跳定时器
Input: void
Return: void
*************************************************/
void Task::init(u16 nms){
	timeOneSysTick = nms;
	SysTick->CTRL = 0;//复位
	SysTick->VAL  = 0x00;//清空
	SysTick->LOAD = nms*fac_ms;//nms 1次系统心跳
	SysTick->CTRL = 3;//8分频 中断 使能
}

/*************************************************
Function: Task::add
Description: 新建任务
Input: 
	uid  任务编码 0~TASK_MAXNUM
	func 需要执行的函数
	in   几次系统心跳执行一次   0每次loop循环都执行
	ts   最多执行几次   0永不执行 0xFFFF无限次
	st   几次系统心跳后开始执行 0立即执行
	et   几次系统心跳后结束执行 0永不执行 0xFFFF永不停止
Return: void
*************************************************/
void Task::add(u8 uid, void (*func)(void), u16 in, u16 ts, u16 st, u16 et){
	taskType[uid]=new Task_TypeDef;
	taskType[uid]->uid = uid;
	taskType[uid]->status = READY;
	taskType[uid]->startTime=timeSysTick+st;
	if(et==0xFFFF){
		taskType[uid]->endTime=0xFFFFFFFF;
	}else{
		taskType[uid]->endTime=timeSysTick+et;
	}
	taskType[uid]->func=func;
	taskType[uid]->times=ts;
	taskType[uid]->timesRun=0;
	taskType[uid]->interval=in;
}

/*************************************************
Function: Task::update
Description: 更新任务
Input: 
	uid  任务编码 0~TASK_MAXNUM
	func 需要执行的函数
	in   几次系统心跳执行一次   0每次loop循环都执行
	ts   最多执行几次   0永不执行 0xFFFF无限次
	st   几次系统心跳后开始执行 0立即执行
	et   几次系统心跳后结束执行 0永不执行 0xFFFF永不停止
Return: void
*************************************************/
void Task::update(u8 uid, void (*func)(void), u16 in, u16 ts, u16 st, u16 et){
	taskType[uid]->status = READY;
	taskType[uid]->startTime=timeSysTick+st;
	if(et==0xFFFF){
		taskType[uid]->endTime=0xFFFFFFFF;
	}else{
		taskType[uid]->endTime=timeSysTick+et;
	}
	taskType[uid]->func=func;
	taskType[uid]->times=ts;
	taskType[uid]->timesRun=0;
	taskType[uid]->interval=in;
}
/*************************************************
Function: Task::cmd
Description: 控制任务状态
Input: 
	uid    任务编码 0~TASK_MAXNUM
	status 状态
Return: void
*************************************************/
void Task::cmd(u8 uid, Task_Status status){
	taskType[uid]->status = status;
}

/*************************************************
Function: Task::run
Description: 执行任务
Input: void
Return: void
*************************************************/
void Task::run(void){
	for(u16 i=0; i<TASK_MAXNUM; i++){
		if(taskType[i]->status == RUN){
			if(taskType[i]->interval == 0){
				if((timeSysTick >= taskType[i]->startTime) && (timeSysTick < taskType[i]->endTime)){
					if(taskType[i]->times==0xFFFF || taskType[i]->timesRun < taskType[i]->times){
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
Description: 心跳定时器中断(每timeOneSysTick毫秒触发1次)
Input: void
Return: void
*************************************************/
_C void SysTick_Handler(void){
	timeSysTick++;
	for(u16 i=0; i<TASK_MAXNUM; i++){
		if(task.taskType[i]->status == task.READY || task.taskType[i]->status == task.FINISH){
			if((timeSysTick >= task.taskType[i]->startTime) && (timeSysTick < task.taskType[i]->endTime)){
				if(task.taskType[i]->times==0xFFFF || task.taskType[i]->timesRun < task.taskType[i]->times){
					if(task.taskType[i]->interval == 0){
						task.taskType[i]->status = task.RUN;
					}else if((timeSysTick-task.taskType[i]->startTime)%task.taskType[i]->interval==0){
						task.taskType[i]->status = task.RUN;
					}
				}
			}
		}
	}
}
