#ifndef __RCC_H
#define __RCC_H
#include "sys.hpp"

#define AHB_DMA1    0x00000001
#define AHB_DMA2    0x00000002
#define AHB_SRAM    0x00000004
#define AHB_FLITF   0x00000010
#define AHB_CRC     0x00000040
#define AHB_FSMC    0x00000100
#define AHB_SDIO    0x00000400

#define APB2_AFIO   0x00000001
#define APB2_GPIOA  0x00000004
#define APB2_GPIOB  0x00000008
#define APB2_GPIOC  0x00000010
#define APB2_GPIOD  0x00000020
#define APB2_GPIOE  0x00000040
#define APB2_GPIOF  0x00000080
#define APB2_GPIOG  0x00000100
#define APB2_ADC1   0x00000200
#define APB2_ADC2   0x00000400
#define APB2_TIM1   0x00000800
#define APB2_SPI1   0x00001000
#define APB2_TIM8   0x00002000
#define APB2_USART1 0x00004000
#define APB2_ADC3   0x00008000

#define APB1_TIM2   0x00000001
#define APB1_TIM3   0x00000002
#define APB1_TIM4   0x00000004
#define APB1_TIM5   0x00000008
#define APB1_TIM6   0x00000010
#define APB1_TIM7   0x00000020
#define APB1_WWDG   0x00000800
#define APB1_SPI2   0x00004000
#define APB1_SPI3   0x00008000
#define APB1_USART2 0x00020000
#define APB1_USART3 0x00040000
#define APB1_UART4  0x00080000
#define APB1_UART5  0x00100000
#define APB1_I2C1   0x00200000
#define APB1_I2C2   0x00400000
#define APB1_USB    0x00800000
#define APB1_CAN    0x02000000
#define APB1_BKP    0x08000000
#define APB1_PWR    0x10000000
#define APB1_DAC    0x20000000

//时钟定义
extern u32 sysclk;
extern u32 ahbclk;
extern u32 apb1clk;
extern u32 apb2clk;

namespace rcc{
	void cmd(u8 bus, u32 periph, FunctionalState state);
	void reset(u8 bus, u32 periph);
	void init(u8 PLL);//时钟初始化
}
#endif //__RCC_H
