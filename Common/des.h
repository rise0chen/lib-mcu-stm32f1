#ifndef __DES_H
#define __DES_H
#include <stdint.h>

void des(uint8_t*, uint8_t*, uint8_t, uint8_t*);
static void transpose(uint8_t*, uint8_t*, const uint8_t*, uint8_t);
static void rotate_l(uint8_t*);
static void compute_subkeys(uint8_t*);
static void f(uint8_t*, uint8_t*, uint8_t*);

#endif  //__DES_H
