#include "./randomSeed.h"
#include "./flash.h"

int randomSeed(void) {
  uint32_t seed = 0;
  uint32_t dwtCycCnt = *(volatile uint32_t*)0xE0001004;
  uint32_t sysTickCnt = (SysTick->VAL & 0x0FFF) | ((SysTick->VAL) << 24);
  uint8_t ID[16];
  flash.read(0xE000ED00, &ID[0], 4);   //读CPUID
  flash.read(0X1FFFF7E8, &ID[4], 12);  //读设备ID

  seed = sysTickCnt ^ ((dwtCycCnt << 16) | (dwtCycCnt >> 16));
  seed ^= (uint32_t) * (&ID[0 + (dwtCycCnt & 0x03)]);
  seed ^= (uint32_t) * (&ID[3 + (sysTickCnt & 0x03)]);
  seed ^= (uint32_t) * (&ID[6 + (dwtCycCnt & 0x03)]);
  seed ^= (uint32_t) * (&ID[9 + (sysTickCnt & 0x03)]);
  seed &= 0x7fffffff;
 
  return (int)seed;
}