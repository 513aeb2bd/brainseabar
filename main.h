#if !defined BSBHEAD_ENTRY
#   define BSBHEAD_ENTRY

// #   define NDEBUG
#   include <stdint.h>
#   include <assert.h>

#   define BSB_LENSTACK_INIT   8   // 0x000008
#   define BSB_LENSTACK_MAX    0x10000

#   define SYM_ADD           'l'
#   define SYM_COMMENT       '#'
#   define SYM_COMMENT_END   '\n'
#   define SYM_COPY          'I'
#   define SYM_EXCHANGE      'O'
#   define SYM_INPUT         'i'
#   define SYM_MOVETO_ST0    '\"'
#   define SYM_MOVETO_ST1    '\''
#   define SYM_NAND          '|'
#   define SYM_PAREN_CLOSE   ']'
#   define SYM_PAREN_OPEN    '['
#   define SYM_POP           '0'
#   define SYM_PRINT_ASCII   'j'
#   define SYM_PRINT_NUM     'J'
#   define SYM_PUSH1         '1'

enum {
   BSB_INST_UNDEF      ,
   BSB_INST_PUSH1      ,
   BSB_INST_POP        ,
   BSB_INST_MOVETO_ST1 ,
   BSB_INST_MOVETO_ST0 ,
   BSB_INST_COPY       ,
   BSB_INST_ADD        ,
   BSB_INST_NAND       ,
   BSB_INST_INPUT      ,
   BSB_INST_PRINT_ASCII,
   BSB_INST_PAREN_OPEN ,
   BSB_INST_PAREN_CLOSE,
   BSB_INST_COMMENT    ,
   BSB_INST_COMMENT_END,
   BSB_INST_PRINT_NUM  ,
   BSB_INST_EXCHANGE   ,
   BSB_NINST           ,
};   // endenum: instruction symbol

typedef int8_t     ib;
typedef uint8_t    ub;
typedef int32_t    id;
typedef uint32_t   ud;
typedef int64_t    iq;
typedef uint64_t   uq;

struct bsbstack {   // main.h
   ub *st0;
   ub *st1;
   uq sp0;
   uq sp1;
   uq spmax0;
   uq spmax1;
};   // endstruct: bsbstack

union inststat {   // main.h
   struct {
      ub inc_st0 : 1;   // increase st0 pointer
      ub dec_st0 : 1;   // decrease st0 pointer
      ub inc_st1 : 1;   // increase st1 pointer
      ub dec_st1 : 1;   // decrease st1 pointer
      ub parenth : 1;   // paren open or close
      ub deprec  : 1;   // deprecated
   };
   struct {
      ub mov_sp0 : 2;   // moves st0 pointer
      ub mov_sp1 : 2;   // moves st1 pointer
   };
};   // endunion: inststat

#endif   // !def ENTRY
