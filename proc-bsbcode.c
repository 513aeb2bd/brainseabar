#include "main.h"

#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

// #####################################################################
// #####################################################################

id proc_bsbcode (ib *inst, id *paren);

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
proc_bsbcode (ib *inst, id *paren)
{
   ub *bsbst0, *bsbst1;
   id sp0, sp1, lenst0, lenst1;
   ub chtarg;

   assert (inst != NULL && paren != NULL);

   lenst0 = lenst1 = 32;

   bsbst0 = malloc (lenst0 * sizeof *bsbst0);
   bsbst1 = malloc (lenst1 * sizeof *bsbst1);

   if (bsbst0 == NULL || bsbst1 == NULL)  goto bran_error_alloc;

   sp0 = sp1 = 0;
   for (ib *iinst = inst; *iinst != BSB_INST_UNDEF; iinst += 1) {
      // until undef instruction
      if (flag_inst[*iinst].inc_st0) {
         if (sp0 == lenst0) {
            lenst0 <<= 1;   // double new stack size
            bsbst0 = realloc (bsbst0, lenst0 * sizeof *bsbst0);

            if (bsbst0 == NULL)  goto bran_error_alloc;
         }   // endif: stack0 is full
      }   // endif: item is pushed to stack0
      else if (flag_inst[*iinst].dec_st0 && sp0 == 0)
         goto bran_error_sp_range;
      // endif: item is popped from stack0 but stack0 is empty

      // now st0 size no problem

      if (flag_inst[*iinst].inc_st1) {
         if (sp1 == lenst1) {
            lenst1 <<= 1;   // double new stack size
            bsbst1 = realloc (bsbst1, lenst1 * sizeof *bsbst1);

            if (bsbst1 == NULL)  goto bran_error_alloc;
         }   // endif: stack1 is full
      }   // endif: item is pushed to stack1
      else if (flag_inst[*iinst].dec_st1 && sp1 == 0)
         goto bran_error_sp_range;
      // endif: item is popped from stack1 but stack1 is empty

      // now st1 size no problem

      switch (*iinst % BSB_NINST) {
      case BSB_INST_PUSH1:
         bsbst0[sp0] = 1;
         sp0 += 1;
         break;
      case BSB_INST_POP:
         sp0 -= 1;
         break;
      case BSB_INST_MOVETO_ST1:
         sp0 -= 1;
         bsbst1[sp1] = bsbst0[sp0];
         sp1 += 1;
         break;   // endcase: INST-MOVETO-ST1
      case BSB_INST_MOVETO_ST0:
         sp1 -= 1;
         bsbst0[sp0] = bsbst1[sp1];
         sp0 += 1;
         break;   // endcase: INST-MOVETO-ST0
      case BSB_INST_COPY:
         bsbst0[sp0] = bsbst0[sp0 - 1];
         sp0 += 1;
         break;
      case BSB_INST_ADD:
         sp0 -= 1;
         bsbst0[sp0 - 1] += bsbst0[sp0];
         break;
      case BSB_INST_NAND:
         sp0 -= 1;
         bsbst0[sp0 - 1] = ~(bsbst0[sp0 - 1] & bsbst0[sp0]);
         break;
      case BSB_INST_INPUT:
         printf (">> ");
         scanf ("%hhu", bsbst0 + sp0);
         getc (stdin);  // act as fflush
         sp0 += 1;
         break;   // endcase: INST-INPUT
      case BSB_INST_PRINT_ASCII:
      case BSB_INST_PRINT_NUM:
         chtarg = bsbst0[sp0 - 1];
         printf ("%d%d%d%d%d%d%d%d\n",
               !!(chtarg & 0x80u), !!(chtarg & 0x40u),
               !!(chtarg & 0x20u), !!(chtarg & 0x10u),
               !!(chtarg & 0x08u), !!(chtarg & 0x04u),
               !!(chtarg & 0x02u), !!(chtarg & 0x01u));
         break;   // endcase: INST-PRINT-ASCII, INST-PRINT-NUM
      case BSB_INST_PAREN_OPEN:
         if (bsbst0[sp0 - 1] != 0)  break;

         iinst = inst + paren[iinst - inst];
         break;   // endcase: INST-PAREN-OPEN
      case BSB_INST_PAREN_CLOSE:
         if (bsbst0[sp0 - 1] == 0)  break;

         iinst = inst + paren[iinst - inst];
         break;   // endcase: INST-PAREN-CLOSE
      case BSB_INST_EXCHANGE:
         chtarg = bsbst0[sp0 - 1];
         bsbst0[sp0 - 1] = bsbst0[sp0 - 2];
         bsbst0[sp0 - 2] = chtarg;
         break;
      default:
         break;
      }   // endswitch: for instruction pointed
   }   // endfor: until undef instruction

   free (bsbst0);
   free (bsbst1);
   return 1;

bran_error_sp_range:
bran_error_alloc:

   free (bsbst0);
   free (bsbst1);
   return 0;
}   // endfunc: void proc-bsbcode
