#ifndef __FONT_H
#define __FONT_H
#include "sys.h"

#define ASCII8x16

typedef struct {
  const uint8_t* table;
  uint8_t width;
  uint8_t height;
  uint16_t size;
} font_TypeDef;
typedef struct {
  uint8_t* data;
  uint8_t width;
  uint8_t height;
} canvas_TypeDef;

#ifdef ASCII8x16
extern font_TypeDef FontAscii8x16;
#endif

#endif  // __FONT_H
