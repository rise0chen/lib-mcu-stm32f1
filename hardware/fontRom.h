#ifndef __FONTROM_H
#define __FONTROM_H
#include "spi.h"
#include "sys.h"

class FontRom {
 public:
  FontRom(Spi* spi_com, Gpio* spi_cs);
  void init(void);
  void getWord16x16(uint16_t gbk, uint8_t* data);
  void getWord8x16(uint8_t ascii, uint8_t* data);
  unsigned char utf8ToGbk(const char* bufIn, char* bufOut);

 private:
  Spi* com;
  Gpio* CS;
};

#endif /* __FONTS_H */
