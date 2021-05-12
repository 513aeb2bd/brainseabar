#ifndef BSBENTRY 

#define BSBENTRY

#include <stdint.h>

#define STACKSIZE_INIT  64
#define STACKSIZE_MAX  0x100000

typedef int8_t  i8;
typedef uint8_t  ui8;
typedef int32_t  i32;
typedef uint32_t  ui32;
typedef uint64_t  ui64;

struct bsbstack {
  ui8* st0;
  ui8* st1;
  ui64 sp0;
  ui64 sp1;
  ui64 spmax0;
  ui64 spmax1;
};

#endif
