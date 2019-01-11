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
#include "task.h"

#define fac_us (9)  //us倍乘数 (rcc_clkSys/8 000 000)
#define fac_ms (fac_us*1000)  //ms倍乘数

uint16_t timeOneSysTick = 0;//1次系统心跳的时间(ms)
uint32_t timeSysTick = 0;//系统心跳次数

TaskStruct* tasks[TASK_NUM_MAX];//任务列表

/*************************************************
Function: task_init
Description: 初始化SysTick心跳定时器
Input: void
Return: void
*************************************************/
void task_init(uint16_t nms){
	timeOneSysTick = nms;
	SysTick->CTRL = 0;//复位
	SysTick->VAL  = 0x00;//清空
	SysTick->LOAD = nms*fac_ms;//nms 1次系统心跳
	SysTick->CTRL = 3;//8分频 中断 使能
}

/*************************************************
Function: task_add
Description: 新建任务
Input: 
	uid  任务编码 0~TASK_NUM_MAX
	func 需要执行的函数
	in   几次系统心跳执行一次   0每次loop循环都执行
	ts   最多执行几次   0永不执行 0xFFFF无限次
	st   几次系统心跳后开始执行 0立即执行
	et   几次系统心跳后结束执行 0永不执行 0xFFFF永不停止
Return: void
*************************************************/
void task_add(uint8_t uid, void (*func)(void), uint16_t in, uint16_t ts, uint16_t st, uint16_t et){
	tasks[uid]=(TaskStruct*)malloc(sizeof(TaskStruct));
	tasks[uid]->uid = uid;
	tasks[uid]->status = taskStatus_ready;
	tasks[uid]->startTime=timeSysTick+st;
	if(et==0xFFFF){
		tasks[uid]->endTime=0xFFFFFFFF;
	}else{
		tasks[uid]->endTime=timeSysTick+et;
	}
	tasks[uid]->func=func;
	tasks[uid]->times=ts;
	tasks[uid]->timesRun=0;
	tasks[uid]->interval=in;
}

/*************************************************
Function: task_update
Description: 更新任务
Input: 
	uid  任务编码 0~TASK_NUM_MAX
	func 需要执行的函数
	in   几次系统心跳执行一次   0每次loop循环都执行
	ts   最多执行几次   0永不执行 0xFFFF无限次
	st   几次系统心跳后开始执行 0立即执行
	et   几次系统心跳后结束执行 0永不执行 0xFFFF永不停止
Return: void
*************************************************/
void task_update(uint8_t uid, void (*func)(void), uint16_t in, uint16_t ts, uint16_t st, uint16_t et){
	tasks[uid]->status = taskStatus_ready;
	tasks[uid]->startTime=timeSysTick+st;
	if(et==0xFFFF){
		tasks[uid]->endTime=0xFFFFFFFF;
	}else{
		tasks[uid]->endTime=timeSysTick+et;
	}
	tasks[uid]->func=func;
	tasks[uid]->times=ts;
	tasks[uid]->timesRun=0;
	tasks[uid]->interval=in;
}
/*************************************************
Function: task_cmd
Description: 控制任务状态
Input: 
	uid    任务编码 0~TASK_NUM_MAX
	status 状态
Return: void
*************************************************/
void task_cmd(uint8_t uid, TaskStatusEnum status){
	tasks[uid]->status = status;
}

/*************************************************
Function: task_run
Description: 执行任务
Input: void
Return: void
*************************************************/
void task_run(void){
	for(uint16_t i=0; i<TASK_NUM_MAX; i++){
		if(tasks[i]->status == taskStatus_run){
			if(tasks[i]->interval == 0){
				if((timeSysTick >= tasks[i]->startTime) && (timeSysTick < tasks[i]->endTime)){
					if(tasks[i]->times==0xFFFF || tasks[i]->timesRun < tasks[i]->times){
						tasks[i]->timesRun++;
						tasks[i]->func();
					}
				}
			}else{
				tasks[i]->timesRun++;
				tasks[i]->func();
				tasks[i]->status = taskStatus_finish;
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
void SysTick_Handler(void){
	timeSysTick++;
	for(uint16_t i=0; i<TASK_NUM_MAX; i++){
		if(tasks[i]->status == taskStatus_ready || tasks[i]->status == taskStatus_finish){
			if((timeSysTick >= tasks[i]->startTime) && (timeSysTick < tasks[i]->endTime)){
				if(tasks[i]->times==0xFFFF || tasks[i]->timesRun < tasks[i]->times){
					if(tasks[i]->interval == 0){
						tasks[i]->status = taskStatus_run;
					}else if((timeSysTick-tasks[i]->startTime)%tasks[i]->interval==0){
						tasks[i]->status = taskStatus_run;
					}
				}
			}
		}
	}
}
