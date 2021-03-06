/*************************************************
Copyright (C), 2018-2028, Crise Tech. Co., Ltd.
File name: Can.cpp
Author: rise0chen
Version: 1.0
Date: 2018.4.26
Description: CAN总线 类
Usage:
        #include "Can.h"
        can.init();
        can.configFilter(0x00, 1, 0x00000000, 0x00000000); //屏蔽模式 接收所有
        can.send(&can.tx);
        can.rcv(&can.rx);
History:
        rise0chen   2018.4.26   初步完成
*************************************************/
#include "can.h"

Can can;

/*************************************************
Function: Can::init
Description: 初始化CAN总线
Calls:
Called By:
Input: void
Return: 通用错误码
*************************************************/
ErrorStatus Can::init(void) {
  Gpio(PA, 11).config(P_UIN);   // RX
  Gpio(PA, 12).config(P_PPAF);  // TX
  rcc.cmd(1, APB1_CAN, ENABLE);
  rcc.reset(1, APB1_CAN);

  CAN1->MCR &= (~(uint32_t)CAN_MCR_SLEEP);  //退出睡眠
  CAN1->MCR |= CAN_MCR_INRQ;                //请求进入初始化模式
  reTry = 0xFFFF;
  while ((CAN1->MSR & CAN_MSR_INAK) != CAN_MSR_INAK) {  //等待同意
    if (reTry-- <= 0)
      return error_other;
  }
  CAN1->MCR &= ~(uint32_t)CAN_MCR_TTCM;  //关闭时间触发通信模式使能
  CAN1->MCR |= CAN_MCR_ABOM;             //自动离线管理
  CAN1->MCR |= CAN_MCR_AWUM;             //使用自动唤醒模式
  CAN1->MCR &= ~(uint32_t)CAN_MCR_NART;  //禁止报文自动重传 DISABLE-自动重传
  CAN1->MCR &=
      ~(uint32_t)
          CAN_MCR_RFLM;  //接收FIFO 锁定模式  DISABLE-溢出时新报文会覆盖原有报文
  CAN1->MCR &=
      ~(uint32_t)CAN_MCR_TXFP;  //发送FIFO优先级 DISABLE-优先级取决于报文标示符
  //定义了时间单元的时间长度 36000k/(1+5+3)/40=100k bps
  //        = 正常模式              重新同步跳跃宽度(2个时间单元)
  //        时间段1(5个时间单元)      时间段2(3个时间单元) 波特率分频器(40分频)
  CAN1->BTR = ((uint32_t)0x00 << 30) | ((uint32_t)0x01 << 24) |
              ((uint32_t)0x04 << 16) | ((uint32_t)0x02 << 20) |
              ((uint32_t)40 - 1);
  CAN1->MCR &= ~(uint32_t)CAN_MCR_INRQ;  //请求退出初始化模式
  reTry = 0xFFFF;
  while ((CAN1->MSR & CAN_MSR_INAK) == CAN_MSR_INAK) {
    if (reTry-- <= 0)
      return error_other;
  }
  return error_success;
}

/*************************************************
Function: Can::configFilter
Description: 设置过滤器
Calls:
Called By:
Input:
        number   过滤器编号 0~13
        maskMode 1屏蔽模式 0列表模式
        ID       标识码(使用软件计算)
        mask     屏蔽码(使用软件计算)
Return: void
*************************************************/
void Can::configFilter(uint8_t number,
                       uint8_t maskMode,
                       uint32_t ID,
                       uint32_t mask) {
  uint32_t filter_number_bit_pos = (uint32_t)((uint32_t)1) << number;

  CAN1->FMR |= 0x00000001;                   //进入过滤器初始化模式
  CAN1->FA1R &= ~filter_number_bit_pos;      // Deactivation
  CAN1->FS1R |= filter_number_bit_pos;       // scale
  CAN1->sFilterRegister[number].FR1 = ID;    //标识码
  CAN1->sFilterRegister[number].FR2 = mask;  //屏蔽码
  if (maskMode) {                            //屏蔽模式
    CAN1->FM1R &= ~(uint32_t)filter_number_bit_pos;
  } else {  //列表模式
    CAN1->FM1R |= (uint32_t)filter_number_bit_pos;
  }
  // CAN1->FFA1R &= ~(uint32_t)filter_number_bit_pos;//FIFO0
  CAN1->FFA1R |= (uint32_t)filter_number_bit_pos;  // FIFO1
  CAN1->FA1R |= filter_number_bit_pos;             //使能过滤器
  CAN1->FMR &= ~0x00000001;  //退出过滤器初始化模式

  // nvic.config(USB_LP_CAN1_RX0_IRQn, 0 ,0);//使用FIFO0
  nvic.config(CAN1_RX1_IRQn, 0, 0);  //使用FIFO1
  // CAN1->IER |= 0x00000002; //FIFO0消息中断
  CAN1->IER |= 0x00000010;  // FIFO1消息中断
}

/*************************************************
Function: Can::send
Description: 发送CAN总线数据
Calls:
Called By:
Input:
        msg  数据
Return: void
*************************************************/
uint8_t Can::send(Msg_TypeDef* msg) {
  uint8_t transmit_mailbox = 0;
  if ((CAN1->TSR & CAN_TSR_TME0) == CAN_TSR_TME0) {
    transmit_mailbox = 0;
  } else if ((CAN1->TSR & CAN_TSR_TME1) == CAN_TSR_TME1) {
    transmit_mailbox = 1;
  } else if ((CAN1->TSR & CAN_TSR_TME2) == CAN_TSR_TME2) {
    transmit_mailbox = 2;
  } else {
    transmit_mailbox = 0xff;  //无空闲
  }

  if (transmit_mailbox != 0xff) {
    CAN1->sTxMailBox[transmit_mailbox].TIR &= 0x00000001;  //请求发送
    if (msg->IDE == CAN_Id_Standard) {
      CAN1->sTxMailBox[transmit_mailbox].TIR |=
          ((msg->StdId << 21) | msg->IDE | msg->RTR);
    } else {
      CAN1->sTxMailBox[transmit_mailbox].TIR |=
          ((msg->ExtId << 3) | msg->IDE | msg->RTR);
    }
    CAN1->sTxMailBox[transmit_mailbox].TDTR &= (uint32_t)0xFFFFFFF0;
    CAN1->sTxMailBox[transmit_mailbox].TDTR |= msg->DLC;
    CAN1->sTxMailBox[transmit_mailbox].TDLR =
        *(uint32_t*)&msg->Data[0];  // Data[0~3]
    CAN1->sTxMailBox[transmit_mailbox].TDHR =
        *(uint32_t*)&msg->Data[4];                         // Data[4~7]
    CAN1->sTxMailBox[transmit_mailbox].TIR |= 0x00000001;  //请求发送
  }
  return transmit_mailbox;
}

/*************************************************
Function: Can::rcv
Description: 接收CAN总线数据
Calls:
Called By:
Input:
        msg  数据
Return: void
*************************************************/
void Can::rcv(Msg_TypeDef* msg) {
  uint8_t FIFONumber = 1;
  msg->IDE = (uint8_t)0x04 & CAN1->sFIFOMailBox[FIFONumber].RIR;
  if (msg->IDE == CAN_Id_Standard) {
    msg->StdId =
        (uint32_t)0x000007FF & (CAN1->sFIFOMailBox[FIFONumber].RIR >> 21);
  } else {
    msg->ExtId =
        (uint32_t)0x1FFFFFFF & (CAN1->sFIFOMailBox[FIFONumber].RIR >> 3);
  }
  msg->RTR = (uint8_t)0x02 & CAN1->sFIFOMailBox[FIFONumber].RIR;
  msg->DLC = (uint8_t)0x0F & CAN1->sFIFOMailBox[FIFONumber].RDTR;
  msg->FMI = (uint8_t)0xFF & (CAN1->sFIFOMailBox[FIFONumber].RDTR >> 8);
  msg->Data[0] = (uint8_t)0xFF & (CAN1->sFIFOMailBox[FIFONumber].RDLR);
  msg->Data[1] = (uint8_t)0xFF & (CAN1->sFIFOMailBox[FIFONumber].RDLR >> 8);
  msg->Data[2] = (uint8_t)0xFF & (CAN1->sFIFOMailBox[FIFONumber].RDLR >> 16);
  msg->Data[3] = (uint8_t)0xFF & (CAN1->sFIFOMailBox[FIFONumber].RDLR >> 24);
  msg->Data[4] = (uint8_t)0xFF & (CAN1->sFIFOMailBox[FIFONumber].RDHR);
  msg->Data[5] = (uint8_t)0xFF & (CAN1->sFIFOMailBox[FIFONumber].RDHR >> 8);
  msg->Data[6] = (uint8_t)0xFF & (CAN1->sFIFOMailBox[FIFONumber].RDHR >> 16);
  msg->Data[7] = (uint8_t)0xFF & (CAN1->sFIFOMailBox[FIFONumber].RDHR >> 24);
  if (FIFONumber == 0) {
    CAN1->RF0R |= CAN_RF0R_RFOM0;
  } else {
    CAN1->RF1R |= CAN_RF1R_RFOM1;
  }
}

void Can::test(Usart* usart) {
  this->rcv(&this->rx);
  usart->printf((char*)"ID :%x\r\n", this->rx.ExtId);
  usart->printf((char*)"IDE:%x\r\n", this->rx.IDE);
  usart->printf((char*)"RTR:%x\r\n", this->rx.RTR);
  usart->printf((char*)"DLC:%x\r\n", this->rx.DLC);
  usart->printf((char*)"FMI:%x\r\n", this->rx.FMI);
  for (uint8_t i = 0; i < this->rx.DLC; i++)
    usart->printf((char*)"FMI:%x\r\n", this->rx.Data[i]);
}
