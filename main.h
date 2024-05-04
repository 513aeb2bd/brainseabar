#if !defined BSBENTRY
#   define BSBENTRY

// #   define NDEBUG
#   include <stdint.h>
#   include <assert.h>

#   define STACKSIZE_INIT  64
#   define STACKSIZE_MAX  0x100000
#   define BSB_LEN_STACK_INIT   64
#   define BSB_LEN_STACK_MAX    0x100000

typedef int8_t  i8;
typedef uint8_t  ui8;
typedef int32_t  i32;
typedef uint32_t  ui32;
typedef uint64_t  ui64;

typedef int8_t     ib;
typedef uint8_t    ub;
typedef int32_t    id;
typedef uint32_t   ud;
typedef uint64_t   uq;

struct bsbstack {
   ub *st0;
   ub *st1;
   uq sp0;
   uq sp1;
   uq spmax0;
   uq spmax1;
};

#endif   // !def ENTRY
