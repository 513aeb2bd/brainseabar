#include "main.h"

#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

// #####################################################################
// #####################################################################

id proc_bsbcode (ib *inst, ib **paren);

// .

extern char inst_code2char[BSB_NINST];   // prep-bsbcode.c
extern struct {
   ub parenth : 1;   // paren open or close
   ub inc_st0 : 1;   // increase st0 pointer
   ub inc_st1 : 1;   // increase st1 pointer
   ub dec_st0 : 1;   // decrease st0 pointer
   ub dec_st1 : 1;   // decrease st1 pointer
   ub deprec  : 1;   // deprecated
} flag_inst[BSB_NINST];   // prep-bsbcode.c

// #####################################################################
// #####################################################################

id
proc_bsbcode (ib *inst, ib **paren)
{
   ub *bsbst0, *bsbst1;   // TODO: add dynamic stack size
   ub bst0[32], bst1[32];
   id sp0, sp1, lenst0, lenst1;
   ub chtarg;

   assert (inst != NULL && paren != NULL);

   lenst0 = lenst1 = 32;
   sp0 = sp1 = 0;
   for (ib *iinst = inst; *iinst != BSB_INST_UNDEF; iinst += 1) {
      // until undef instruction
      if (flag_inst[*iinst].inc_st0) {
         if (sp0 == lenst0) {
            return 0;
         }
      }
      else if (flag_inst[*iinst].dec_st0 && sp0 == 0)  return 0;

      // now st0 size no problem

      if (flag_inst[*iinst].inc_st1) {
         if (sp1 == lenst1) {
            return 0;
         }
      }
      else if (flag_inst[*iinst].dec_st1 && sp1 == 0)  return 0;

      // now st1 size no problem

      switch (*iinst % BSB_NINST) {
      case BSB_INST_PUSH1:
         bst0[sp0] = 1;
         sp0 += 1;
         break;
      case BSB_INST_POP:
         sp0 -= 1;
         break;
      case BSB_INST_MOVETO_ST1:
         sp0 -= 1;
         bst1[sp1] = bst0[sp0];
         sp1 += 1;
         break;   // endcase: INST-MOVETO-ST1
      case BSB_INST_MOVETO_ST0:
         sp1 -= 1;
         bst0[sp0] = bst1[sp1];
         sp0 += 1;
         break;   // endcase: INST-MOVETO-ST0
      case BSB_INST_COPY:
         bst0[sp0] = bst0[sp0 - 1];
         sp0 += 1;
         break;
      case BSB_INST_ADD:
         sp0 -= 1;
         bst0[sp0 - 1] += bst0[sp0];
         break;
      case BSB_INST_NAND:
         sp0 -= 1;
         bst0[sp0 - 1] = ~(bst0[sp0 - 1] & bst0[sp0]);
         break;
      case BSB_INST_INPUT:
         printf (">> ");
         scanf ("%hhu", bst0 + sp0);
         getc (stdin);  // act as fflush
         sp0 += 1;
         break;   // endcase: INST-INPUT
      case BSB_INST_PRINT_ASCII:
      case BSB_INST_PRINT_NUM:
         chtarg = bst0[sp0 - 1];
         printf ("%d%d%d%d%d%d%d%d\n",
               !!(chtarg & 0x80u), !!(chtarg & 0x40u),
               !!(chtarg & 0x20u), !!(chtarg & 0x10u),
               !!(chtarg & 0x08u), !!(chtarg & 0x04u),
               !!(chtarg & 0x02u), !!(chtarg & 0x01u));
         break;   // endcase: INST-PRINT-ASCII, INST-PRINT-NUM
      case BSB_INST_PAREN_OPEN:
         if (bst0[sp0 - 1] != 0)  break;

         iinst = paren[iinst - inst];
         break;   // endcase: INST-PAREN-OPEN
      case BSB_INST_PAREN_CLOSE:
         if (bst0[sp0 - 1] == 0)  break;

         iinst = paren[iinst - inst];
         break;   // endcase: INST-PAREN-CLOSE
      case BSB_INST_EXCHANGE:
         break;
      default:
         break;
      }   // endswitch: for instruction pointed
   }   // endfor: until undef instruction

   return 1;
}   // endfunc: void proc-bsbcode
