#include "main.h"

#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <assert.h>
#include <unistd.h>
#include <sys/stat.h>

// #####################################################################
// #####################################################################

id prep_bsbcode (char *filename, ib **o_inst, id **o_paren);

static void conv_char2inst (ib *o_inst);
static id prep_paren (ib *inst, id *paren);

// .

char inst_code2char[BSB_NINST] = {
   [BSB_INST_PUSH1      ] = '1',
   [BSB_INST_POP        ] = '0',
   [BSB_INST_MOVETO_ST1 ] = '\'',
   [BSB_INST_MOVETO_ST0 ] = '\"',
   [BSB_INST_COPY       ] = 'I',
   [BSB_INST_ADD        ] = 'l',
   [BSB_INST_NAND       ] = '|',
   [BSB_INST_INPUT      ] = 'i',
   [BSB_INST_PRINT_ASCII] = 'j',
   [BSB_INST_PAREN_OPEN ] = '[',
   [BSB_INST_PAREN_CLOSE] = ']',
   [BSB_INST_COMMENT    ] = '#',
   [BSB_INST_PRINT_NUM  ] = 'J',
   [BSB_INST_EXCHANGE   ] = 'O',
};   // endarr: char inst-code2char[]

char *inst_desc[BSB_NINST] = {
   [BSB_INST_UNDEF      ] = "Undefined",
   [BSB_INST_PUSH1      ] = "Push 1 to stack",
   [BSB_INST_POP        ] = "Pop from stack",
   [BSB_INST_MOVETO_ST1 ] = "Move stack pointer left",
   [BSB_INST_MOVETO_ST0 ] = "Move stack pointer right",
   [BSB_INST_COPY       ] = "Push value pointed by stack pointer",
   [BSB_INST_ADD        ] = "Pop 2 items and push their addition",
   [BSB_INST_NAND       ] = "Pop 2 items and push their bitwise NAND",
   [BSB_INST_INPUT      ] = "Push value from input",
   [BSB_INST_PRINT_ASCII] = "Print",
   [BSB_INST_PAREN_OPEN ] = "Parenthesis open",
   [BSB_INST_PAREN_CLOSE] = "Parenthesis close",
   [BSB_INST_COMMENT    ] = "Comment until end of line",
   [BSB_INST_COMMENT_END] = "",
   [BSB_INST_PRINT_NUM  ] = "Print",
   [BSB_INST_EXCHANGE   ] = "Exchange value with left one",
};   // endarr: char *inst-desc[]

struct {
   ub parenth : 1;   // paren open or close
   ub inc_st0 : 1;   // increase st0 pointer
   ub inc_st1 : 1;   // increase st1 pointer
   ub dec_st0 : 1;   // decrease st0 pointer
   ub dec_st1 : 1;   // decrease st1 pointer
   ub deprec  : 1;   // deprecated
} flag_inst[BSB_NINST] = {
   [BSB_INST_PUSH1      ] = { .inc_st0 = 1, },
   [BSB_INST_POP        ] = { .dec_st0 = 1, },
   [BSB_INST_MOVETO_ST1 ] = { .inc_st1 = 1, .dec_st0 = 1, },
   [BSB_INST_MOVETO_ST0 ] = { .inc_st0 = 1, .dec_st1 = 1, },
   [BSB_INST_COPY       ] = { .inc_st0 = 1, },
   [BSB_INST_ADD        ] = { .dec_st0 = 1, },
   [BSB_INST_NAND       ] = { .dec_st0 = 1, },
   [BSB_INST_INPUT      ] = { .inc_st0 = 1, },
   [BSB_INST_PAREN_OPEN ] = { .parenth = 1, },
   [BSB_INST_PAREN_CLOSE] = { .parenth = 1, },
   [BSB_INST_PRINT_NUM  ] = { .deprec  = 1, },
   [BSB_INST_EXCHANGE   ] = { .deprec  = 1, },
};   // endarr: struct @ flag_inst[]

// #####################################################################
// #####################################################################

id
prep_bsbcode (char *filename, ib **o_inst, id **o_paren)
{
   id *paren;
   ib *inst;
   struct stat stat_file;
   id fdbsb, filesize;

   assert (filename != NULL && o_inst != NULL && o_paren != NULL);

   fdbsb = open (filename, O_RDONLY);
   if (fdbsb < 0)  return 0;

   // now file opened

   if (fstat (fdbsb, &stat_file) < 0)  goto ret_error_fstat;

   // now file size is known

   // copy source code to memory
   filesize = stat_file.st_size;
   inst = malloc ((filesize + 1) * sizeof *inst);
   paren = malloc ((filesize + 1) * sizeof *paren);

   if (inst == NULL || paren == NULL)  goto ret_error_alloc;

   // now memory allocated

   read (fdbsb, inst, filesize);
   inst[filesize] = '\0';

   conv_char2inst (inst);

   if (!prep_paren (inst, paren)) {
#if !defined NDEBUG
      printf ("error: wrong parentheses usage\n");
#endif
      goto ret_error_paren;
   }

   // now parentheses correct

   close (fdbsb);

   *o_inst = inst;
   *o_paren = paren;
   return 1;

ret_error_paren:
ret_error_alloc:

   free (inst);
   free (paren);

ret_error_fstat:

   close (fdbsb);
   return 0;
}   // endfunc: id prep-bsbcode

// #####################################################################
// #####################################################################

static ib inst_char2code[256] = {
   [SYM_PUSH1      ] = BSB_INST_PUSH1      ,
   [SYM_POP        ] = BSB_INST_POP        ,
   [SYM_MOVETO_ST1 ] = BSB_INST_MOVETO_ST1 ,
   [SYM_MOVETO_ST0 ] = BSB_INST_MOVETO_ST0 ,
   [SYM_COPY       ] = BSB_INST_COPY       ,
   [SYM_EXCHANGE   ] = BSB_INST_EXCHANGE   ,
   [SYM_ADD        ] = BSB_INST_ADD        ,
   [SYM_NAND       ] = BSB_INST_NAND       ,
   [SYM_INPUT      ] = BSB_INST_INPUT      ,
   [SYM_PRINT_ASCII] = BSB_INST_PRINT_ASCII,
   [SYM_PRINT_NUM  ] = BSB_INST_PRINT_NUM  ,
   [SYM_PAREN_OPEN ] = BSB_INST_PAREN_OPEN ,
   [SYM_PAREN_CLOSE] = BSB_INST_PAREN_CLOSE,
   [SYM_COMMENT    ] = BSB_INST_COMMENT    ,
   [SYM_COMMENT_END] = BSB_INST_COMMENT_END,
};   // endarr: static id inst-char-to-code[]

// second idx indicates prev comment state (comment or no comment)
static ib switch_comment[BSB_NINST][2] = {
   [BSB_INST_COMMENT    ] = { [0] = 1, [1] = 0, },
   [BSB_INST_COMMENT_END] = { [0] = 0, [1] = 1, },
};   // endarr: static id switch-comment[][]

// .

static void
conv_char2inst (ib *o_inst)
{
   ib *psrc, *pdst;
   id is_comment;
   ib codecur;

   assert (o_inst != NULL);

   is_comment = 0;

   // convert ascii instruction to instruction number
   // exclude comment

   for (psrc = pdst = o_inst; *psrc != '\0'; psrc += 1) {
      // for each instruction character
      codecur = inst_char2code[*(ub *)psrc];

      if (codecur == BSB_INST_UNDEF)  continue;

      // now instruction defined

      is_comment ^= switch_comment[codecur][is_comment];

      // if comment, do not copy

      if (is_comment ||
            codecur == BSB_INST_COMMENT_END ||
            codecur == BSB_INST_COMMENT)
         continue;

      // now not comment

      *pdst = codecur;   // copy
      pdst += 1;

      if (flag_inst[codecur].deprec) {
         printf ("note: instruction %c (%s) is deprecated\n", *psrc,
               inst_desc[codecur]);
      }

#if !defined NDEBUG
      printf ("%c", inst_code2char[codecur]);
#endif
   }   // endfor: for each instruction character

#if !defined NDEBUG
   printf ("\n");
#endif

   *pdst = BSB_INST_UNDEF;
}   // endfunc: static void conv-char-to-inst

// #####################################################################
// #####################################################################

static id indi_paren[BSB_NINST] = {
   [BSB_INST_PAREN_OPEN ] = 1,
   [BSB_INST_PAREN_CLOSE] = -1,
};   // endarr: static id indi-paren[]

// .

static id
prep_paren (ib *inst, id *paren)
{
   iq *idx_parenopen;
   id matchparen, cparen, iparen;

   assert (inst != NULL && paren != NULL);

   // check paren match
   // count paren pairs

   matchparen = cparen = 0;
   for (ib *iinst = inst; *iinst != BSB_INST_UNDEF; iinst += 1) {
      // for each instruction
      if (!flag_inst[*iinst].parenth)  continue;

      // now instruction is paren

      matchparen += indi_paren[*iinst];
      cparen += *iinst == BSB_INST_PAREN_OPEN;

      // matchparen cannot be negative
      // check paren close before paren open
      if (matchparen < 0)  return 0;
   }   // endfor: for each instruction

   if (matchparen != 0)  return 0;

   // now paren match

   idx_parenopen = malloc (cparen * sizeof *idx_parenopen);

   if (idx_parenopen == NULL)  return 0;

   // now allocated

   // set jumping address
   iparen = 0;
   for (ib *iinst = inst; *iinst != BSB_INST_UNDEF; iinst += 1) {
      // for each instruction
      if (!flag_inst[*iinst].parenth)  continue;

      // now instruction is paren

      if (*iinst == BSB_INST_PAREN_OPEN) {
         // store index of paren open
         idx_parenopen[iparen] = iinst - inst;
         iparen += 1;
      }   // endif: instruction is paren open
      else {
         iparen -= 1;
         paren[idx_parenopen[iparen]] = iinst - inst;
         paren[iinst - inst] = idx_parenopen[iparen];
#if !defined NDEBUG
         printf ("paren pair: %d[ ]%d\n",
               paren[iinst - inst], paren[idx_parenopen[iparen]]);
#endif
      }   // endif: instruction is paren close
   }   // endfor: for each instruction

   free (idx_parenopen);
   return 1;
}   // endfunc: static id prep-paren
