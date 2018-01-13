#include "sys.hpp"

s32 reTry=0x00FFFFFF;

void SetMem(volatile void* p,u32 where,u32 content){
	u32* p0=(u32*)p;
	*p0 &= ~where;//清零
	*p0 |= content;//置数
}
void SetBit(volatile void* p,u8 where){
	u32* p0=(u32*)p;
	*p0 |= (1<<where);
}
void ClearBit(volatile void* p,u8 where){
	u32* p0=(u32*)p;
	*p0 &= ~(1<<where);
}


//执行times次，直到(*func)()返回0
//auto a = [](u8 b){return b;};//定义局部函数
ErrorStatus run(u8 (*func)(), u32 times){
	reTry	=	times;
	while((*func)()){//判断条件
		if(reTry-- <= 0){return OVERTIME;}
	}
	return SUCCESS;
}























/********汇编*********/
__ASM void MSR_MSP(u32 addr){//设置栈顶地址
//addr:栈顶地址
	MSR MSP, r0//set Main Stack value
	BX r14
}
