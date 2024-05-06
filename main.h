#if !defined BSBENTRY
#   define BSBENTRY

// #   define NDEBUG
#   include <stdint.h>
#   include <assert.h>

#   define STACKSIZE_INIT  64
#   define STACKSIZE_MAX  0x100000
#   define BSB_LENSTACK_INIT   64
#   define BSB_LENSTACK_MAX    0x100000

#define SYM_ADD           'l'
#define SYM_COMMENT       '#'
#define SYM_COMMENT_END   '\n'
#define SYM_COPY          'I'
#define SYM_EXCHANGE      'O'
#define SYM_INPUT         'i'
#define SYM_MOVETO_ST0    '\"'
#define SYM_MOVETO_ST1    '\''
#define SYM_NAND          '|'
#define SYM_PAREN_CLOSE   ']'
#define SYM_PAREN_OPEN    '['
#define SYM_POP           '0'
#define SYM_PRINT_ASCII   'j'
#define SYM_PRINT_NUM     'J'
#define SYM_PUSH1         '1'

enum {
   BSB_INST_UNDEF       = 0x00,
   BSB_INST_PUSH1       = 0x01,
   BSB_INST_POP         = 0x02,
   BSB_INST_MOVETO_ST1  = 0x03,
   BSB_INST_MOVETO_ST0  = 0x04,
   BSB_INST_COPY        = 0x05,
   BSB_INST_ADD         = 0x06,
   BSB_INST_NAND        = 0x07,
   BSB_INST_INPUT       = 0x08,
   BSB_INST_PRINT_ASCII = 0x09,
   BSB_INST_PAREN_OPEN  = 0x0a,
   BSB_INST_PAREN_CLOSE = 0x0b,
   BSB_INST_COMMENT     = 0x0c,
   BSB_INST_COMMENT_END = 0x0d,
   BSB_INST_PRINT_NUM   = 0x0e,
   BSB_INST_EXCHANGE    = 0x0f,
   BSB_NINST            = 0x10,
};   // endenum: instruction symbol

typedef int8_t  i8;
typedef uint8_t  ui8;
typedef int32_t  i32;
typedef uint32_t  ui32;
typedef uint64_t  ui64;

typedef int8_t     ib;
typedef uint8_t    ub;
typedef int32_t    id;
typedef uint32_t   ud;
typedef int64_t    iq;
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
