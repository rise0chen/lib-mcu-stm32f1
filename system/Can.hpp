#ifndef __CAN_H
#define __CAN_H
#include "sys.h"

#define CAN_Id_Standard  ((uint32_t)0x00000000)
#define CAN_Id_Extended  ((uint32_t)0x00000004)
#define CAN_RTR_Data     ((uint32_t)0x00000000)
#define CAN_RTR_Remote   ((uint32_t)0x00000002)

class Can{
	public:
		typedef struct{
			uint32_t StdId;  // 标准ID   0~0x7FF
			uint32_t ExtId;  // 扩展ID   0~0x1FFFFFFF
			uint8_t IDE;     // 传输模式
			uint8_t RTR;     // 数据类型
			uint8_t DLC;     // 数据长度 0~8
			uint8_t Data[8]; // 数据
			uint8_t FMI;     // 过滤器ID 0~0xFF
		} Msg_TypeDef;
		Msg_TypeDef rx;
		Msg_TypeDef tx;
		
		ErrorStatus init(void);
		void configFilter(uint8_t number,uint8_t maskMode, uint32_t ID, uint32_t mask);
		uint8_t send(Msg_TypeDef* msg);
		void rcv(Msg_TypeDef* msg);
		
	private:
};
extern Can can;

#endif //__CAN_H
