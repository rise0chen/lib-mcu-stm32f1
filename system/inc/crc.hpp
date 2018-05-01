#ifndef __CRC_H
#define __CRC_H
#include "sys.hpp"

namespace crc{
	uint16_t get16(char *pBuffer, uint32_t BufferLength);
	uint32_t get32(uint32_t *pBuffer, uint32_t BufferLength);
}

#endif //__CRC_H
