#ifndef __CRC_H
#define __CRC_H
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

uint16_t getCrc16(uint8_t* pBuffer, uint32_t BufferLength);

#ifdef __cplusplus
}
#endif

#endif  //__CRC_H
