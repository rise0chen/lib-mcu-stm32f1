#ifndef __LINK_USART_H
#define __LINK_USART_H
#ifdef __cplusplus
extern "C" {
#endif

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include "fiip-base/fiip.h"
#include "fiip-base/link/link.h"
#include "function.h"

#define linkType_usart 0x31

void startUsart(uint8_t* dev, uint32_t speed);

#ifdef __cplusplus
}
#endif
#endif  //__LINK_USART_H
