#include "delay.hpp"

#define fac_us (72)//us延时倍乘数 (sysclk/1 000 000)
#define fac_ms (fac_us*1000)//ms延时倍乘数

void delay(u16 time){
	u16 i, j;
	for(i=0; i<time; i++)
		for(j=0; j<9000; j++);//1ms
}
void delay_us(u32 nus){
//延时nms<=59652323us
	u32 start,end;
	DEM_CR |= (1<<24);
	DWT_CR |= (1<<0);
	start = DWT_CYCCNT;
	end = DWT_CYCCNT + nus*fac_us;
	if(end > start){
		while(DWT_CYCCNT < end);
	}else{
		while(DWT_CYCCNT > end);
		while(DWT_CYCCNT < end);
	}
}

static u32 startTime;
void tic(){
	DEM_CR |= (1<<24);
	DWT_CR |= (1<<0);
	startTime = DWT_CYCCNT;
}
u32 toc(u8 way){
	u32 dt;
	dt=DWT_CYCCNT-startTime;
	if(way==1){
		dt=dt/fac_us;
	}else if(way==2){
		dt=dt/fac_ms;
	}
	return(dt);
}
