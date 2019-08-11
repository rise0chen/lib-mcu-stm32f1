/*************************************************
Copyright (C), 2018-2028, Crise Tech. Co., Ltd.
File name: it.c
Author: rise0chen
Version: 1.0
Date: 2018.4.26
Description: 中断服务函数
Usage:
History:
        rise0chen   2018.4.26   编写注释
*************************************************/
#include "it.h"

uint32_t tmp;
static VarArrayStruct* Listeners[60];

void it_addListener(uint8_t IRQn, void* fun) {
  if (Listeners[IRQn] == NULL) {
    Listeners[IRQn] = varArray_new();
  }
  varArray_addData(Listeners[IRQn], fun);
}

static void it_run(uint8_t IRQn, uint32_t status) {
  void (*fun)(uint8_t IRQn, uint32_t status);

  for (uint16_t i = 0; i < Listeners[IRQn]->length; i++) {
    fun = (void (*)(uint8_t, uint32_t))Listeners[IRQn]->data[i];
    fun(IRQn, status);
  }
}

void NMI_Handler(void) {}
void HardFault_Handler(void) {
  while (1) {
  }
}
void MemManage_Handler(void) {
  while (1) {
  }
}
void BusFault_Handler(void) {
  while (1) {
  }
}
void UsageFault_Handler(void) {
  while (1) {
  }
}
void SVC_Handler(void) {}
void DebugMon_Handler(void) {}
void PendSV_Handler(void) {}

void USART1_IRQHandler(void) {
  uint32_t status = USART1->SR;
  tmp = USART1->DR;
  USART1->SR = 0;
  it_run(USART1_IRQn, status);
}
void USART2_IRQHandler(void) {
  uint32_t status = USART2->SR;
  tmp = USART2->DR;
  USART2->SR = 0;
  it_run(USART2_IRQn, status);
}
void USART3_IRQHandler(void) {
  uint32_t status = USART3->SR;
  tmp = USART3->DR;
  USART3->SR = 0;
  it_run(USART3_IRQn, status);
}
void CAN1_RX1_IRQHandler() {
  it_run(CAN1_RX1_IRQn, 0);
}
void RTC_IRQHandler(void) {
  uint32_t status = RTC->CRL;
  RTC->CRL = 0;
  it_run(RTC_IRQn, status);
  while (!(RTC->CRL & (1 << 5)))
    ;  //等待RTC寄存器操作完成
}
