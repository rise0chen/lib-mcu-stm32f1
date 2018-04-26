#ifndef __CRC_H
#define __CRC_H
#include "sys.hpp"

namespace crc{
	u16 get16(char *pBuffer, uint32_t BufferLength);  //计算CRC16
	u32 get32(uint32_t pBuffer[], uint32_t BufferLength);  //计算CRC32
}

#endif //__CRC_H
