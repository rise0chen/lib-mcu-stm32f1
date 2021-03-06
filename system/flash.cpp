/*************************************************
Copyright (C), 2018-2028, Crise Tech. Co., Ltd.
File name: Flash.cpp
Author: rise0chen
Version: 1.0
Date: 2018.4.26
Description: Flash 类
Usage:
        #include "Flash.h"
        flash.read(0xE000ED00, ID, 4); //读CPUID
        flash.read(0X1FFFF7E8, ID, 12);//读设备ID
        flash.write(FLASH_ADDR_START, ID, 12);//写入flash
History:
        rise0chen   2018.4.26   编写注释
*************************************************/
#include "flash.h"

Flash flash;

/*************************************************
Function: Flash::Flash
Description: Flash类的构造函数
Calls:
Called By:
Input: void
Return: Flash类
*************************************************/
Flash::Flash() {
#if defined(STM32F10X_HD) || defined(STM32F10X_HD_VL) || \
    defined(STM32F10X_CL) || defined(STM32F10X_XL)
  sizePage = 0x800;  // 2048
#else
  sizePage = 0x400;  // 1024
#endif
}

/*************************************************
Function: Flash::lock
Description: 锁定/解锁flash
Calls:
Called By:
Input: en 0解锁 1锁定
Return: void
*************************************************/
void Flash::lock(uint8_t en) {
  if (en) {
    FLASH->CR |= 0x00000080;
  } else {
    FLASH->KEYR = 0x45670123;
    FLASH->KEYR = 0xCDEF89AB;
  }
}

/*************************************************
Function: Flash::erasePage
Description: 锁定/解锁flash
Calls:
Called By:
Input: en 0解锁 1锁定
Return: void
*************************************************/
void Flash::erasePage(uint32_t paddr) {
  while (FLASH->SR & (1 << 0))
    ;                   //忙
  FLASH->CR |= 1 << 1;  //页擦除
  FLASH->AR = paddr;    //设置页地址
  FLASH->CR |= 1 << 6;  //开始擦除
  while (FLASH->SR & (1 << 0))
    ;                      //忙
  FLASH->CR &= ~(1 << 1);  //清除页擦除标志
}

/*************************************************
Function: Flash::read
Description: 读flash
Calls:
Called By:
Input:
        addr  读取flash的起始地址
        buf   读取flash数据到buf变量中
        num   需要读取的字节数Byte
Return: void
*************************************************/
void Flash::read(uint32_t addr, void* buf, uint16_t num) {
  uint16_t* p = (uint16_t*)buf;

  for (uint16_t i = 0; i < num; i = i + 2) {
    *(p++) = *(uint16_t*)(addr + i);
  }
}

/*************************************************
Function: Flash::write
Description: 写flash
Calls:
Called By:
Input:
        addr  写入flash的起始地址
        buf   写入buf变量到flash中
        num   需要写入的字节数Byte
Return: void
*************************************************/
void Flash::write(uint32_t addr, void* buf, uint16_t num) {
  uint16_t* p = (uint16_t*)buf;
  uint8_t numErase = num / sizePage + 1;

  lock(0);
  for (uint8_t i = 0; i < numErase; i++) {
    erasePage(addr + (sizePage * i));
  }
  for (uint16_t i = 0; i < num; i = i + 2) {
    while (FLASH->SR & (1 << 0))
      ;                   //忙
    FLASH->CR |= 1 << 0;  //编程使能
    *(uint16_t*)(addr + i) = *(p++);
    while (FLASH->SR & (1 << 0))
      ;                      //忙
    FLASH->CR &= ~(1 << 0);  //清除PG位.
  }
  lock(1);
}
