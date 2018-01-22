#include "task.hpp"

#define fac_us (9)//us延时倍乘数 (sysclk/8 000 000)
#define fac_ms (fac_us*1000)//ms延时倍乘数
u32 RunTime;//单位秒

namespace task{
	static task_TypeDef* task_Type[TASK_MAXNUM];
	
	void init(void){
		SysTick->CTRL = 0;//复位
		SysTick->VAL  = 0x00;//清空
		SysTick->LOAD = 1000*fac_ms;//1s心跳
		SysTick->CTRL = 3;//8分频 中断 使能
	}
	void add(u8 uid, void (*func)(void), u8 st, u8 et, u8 in, u8 ts){
		task_Type[uid]=new task_TypeDef;
		task_Type[uid]->uid=uid;
		task_Type[uid]->status=TASK_FINISH;
		task_Type[uid]->startTime=RunTime+st;
		if(et==0){
			task_Type[uid]->endTime=0;
		}else if(et==0xff){
			task_Type[uid]->endTime=0xffffffff;
		}else{
			task_Type[uid]->endTime=RunTime+et;
		}
		task_Type[uid]->func=func;
		task_Type[uid]->times=ts;
		task_Type[uid]->timesRun=0;
		task_Type[uid]->interval=in;
	}
	void update(u8 uid, void (*func)(void), u8 st, u8 et, u8 in, u8 ts){
		task_Type[uid]->status=TASK_FINISH;
		task_Type[uid]->startTime=RunTime+st;
		if(et==0){
			task_Type[uid]->endTime=0;
		}else if(et==0xff){
			task_Type[uid]->endTime=0xffffffff;
		}else{
			task_Type[uid]->endTime=RunTime+et;
		}
		task_Type[uid]->func=func;
		task_Type[uid]->times=ts;
		task_Type[uid]->timesRun=0;
		task_Type[uid]->interval=in;
	}
	void run(void){
		for(u16 i=0; i<256; i++){
			if(task_Type[i]->status == TASK_READY){
				if(task_Type[i]->interval == 0){
					if(RunTime>=task::task_Type[i]->startTime && RunTime<=task::task_Type[i]->endTime){
						if(task::task_Type[i]->times==0 || task::task_Type[i]->timesRun < task::task_Type[i]->times){
							task_Type[i]->timesRun++;
							task_Type[i]->func();
						}
					}
				}else{
					task_Type[i]->timesRun++;
					task_Type[i]->func();
					task_Type[i]->status=TASK_FINISH;
				}
			}
		}
	}
}
_C void SysTick_Handler(void){
	RunTime++;
	for(u16 i=0; i<256; i++){
		if(task::task_Type[i]->status == TASK_FINISH){
			if(RunTime>=task::task_Type[i]->startTime && RunTime<=task::task_Type[i]->endTime){
				if(task::task_Type[i]->times==0 || task::task_Type[i]->timesRun < task::task_Type[i]->times){
					if(task::task_Type[i]->interval == 0){
						task::task_Type[i]->status = TASK_READY;
					}else if((RunTime-task::task_Type[i]->startTime)%task::task_Type[i]->interval==0){
						task::task_Type[i]->status = TASK_READY;
					}
				}
			}
		}
	}
}
