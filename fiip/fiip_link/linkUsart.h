#ifndef __LINK_USART_H
#define __LINK_USART_H
#ifdef __cplusplus
extern "C" {
#endif

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../fiip/fiip.h"
#include "../fiip/link/Link.h"

#define linkType_usart 0x31

void startUsart();

#ifdef __cplusplus
}
#endif
#endif  //__LINK_USART_H
