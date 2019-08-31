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

uint32_t status;
uint32_t tmp;
static VarArrayStruct* Listeners[60];

void it_init(void) {
  flagHandle_init();
}
static void it_run(void* args) {
  uint8_t IRQn = (uint8_t)(int)args;
  void (*fun)(uint8_t IRQn);

  for (uint16_t i = 0; i < Listeners[IRQn]->length; i++) {
    fun = (void (*)(uint8_t))Listeners[IRQn]->data[i];
    fun(IRQn);
  }
}
void it_addListener(uint8_t IRQn, void* fun) {
  flagHandle_addListener(IRQn, (void*)it_run);
  if (Listeners[IRQn] == NULL) {
    Listeners[IRQn] = varArray_new();
  }
  varArray_addData(Listeners[IRQn], fun);
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
  status = USART1->SR;
  tmp = USART1->DR;
  USART1->SR = 0;
  flagHandle_setFlag(USART1_IRQn, (void*)USART1_IRQn);
}
void USART2_IRQHandler(void) {
  status = USART2->SR;
  tmp = USART2->DR;
  USART2->SR = 0;
  flagHandle_setFlag(USART2_IRQn, (void*)USART2_IRQn);
}
void USART3_IRQHandler(void) {
  status = USART3->SR;
  tmp = USART3->DR;
  USART3->SR = 0;
  flagHandle_setFlag(USART3_IRQn, (void*)USART3_IRQn);
}
void CAN1_RX1_IRQHandler() {
  flagHandle_setFlag(CAN1_RX1_IRQn, (void*)CAN1_RX1_IRQn);
}
void RTC_IRQHandler(void) {
  status = RTC->CRL;
  RTC->CRL = 0;
  flagHandle_setFlag(RTC_IRQn, (void*)RTC_IRQn);
  while (!(RTC->CRL & (1 << 5)))
    ;  //等待RTC寄存器操作完成
}
