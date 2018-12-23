#ifndef __ESCAPE_H
#define __ESCAPE_H

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
  unsigned char len[2];
  unsigned char buf[2][8];
} ESCStruct;

unsigned short escape(char* bufin,
                      unsigned short lenin,
                      ESCStruct* type,
                      unsigned char typelen,
                      unsigned char way);

#ifdef __cplusplus
}
#endif

#endif  //__ESCAPE_H
