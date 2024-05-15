#include "main.h"

#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

// #####################################################################
// #####################################################################

id proc_bsbcode (ib *inst, id *paren);

// .

extern char inst_code2char[BSB_NINST];   // prep-bsbcode.c
extern union inststat flag_inst[BSB_NINST];   // prep-bsbcode.c

// #####################################################################
// #####################################################################

id
proc_bsbcode (ib *inst, id *paren)
{
   ub *bsbst0, *bsbst1;
   id sp0, sp1, lenst0, lenst1;
   ub chtarg;

   assert (inst != NULL && paren != NULL);

   lenst0 = lenst1 = BSB_LENSTACK_INIT;

   bsbst0 = malloc (lenst0 * sizeof *bsbst0);
   bsbst1 = malloc (lenst1 * sizeof *bsbst1);

   if (bsbst0 == NULL || bsbst1 == NULL)  goto ret_error_alloc;

   sp0 = sp1 = 0;
   for (ib *iinst = inst; *iinst != BSB_INST_UNDEF; iinst += 1) {
      // until undef instruction

      // check stack0 overflow/underflow

      if (!flag_inst[*iinst].mov_sp0 || sp0 != 0 && sp0 != lenst0)
         goto skip_check_stack0;
      // now sp0 is either empty or full

      if (sp0 == lenst0) {
         if (!flag_inst[*inst].inc_st0)  goto skip_check_stack0;
         // now instruction increases sp0
         if (BSB_LENSTACK_MAX <= lenst0)  goto ret_error_stackrange;
         // now stack length is not max

         lenst0 <<= 1;   // double new stack size
         bsbst0 = realloc (bsbst0, lenst0 * sizeof *bsbst0);

         if (bsbst0 == NULL)  goto ret_error_alloc;

#if !defined NDEBUG
         printf ("stack0 size: %d to %d\n", lenst0 >> 1, lenst0);
#endif
      }   // endif: stack0 is full
      else if (flag_inst[*iinst].dec_st0)  goto ret_error_stackrange;

   skip_check_stack0:

      // check stack1 overflow/underflow

      if (!flag_inst[*iinst].mov_sp1 || sp1 != 0 && sp1 != lenst1)
         goto skip_check_stack1;
      // now sp1 is either empty or full

      if (sp1 == lenst1) {
         if (!flag_inst[*inst].inc_st1)  goto skip_check_stack1;
         // now instruction increases sp1
         if (BSB_LENSTACK_MAX <= lenst1)  goto ret_error_stackrange;
         // now stack length is not max

         lenst1 <<= 1;   // double new stack size
         bsbst1 = realloc (bsbst1, lenst1 * sizeof *bsbst1);

         if (bsbst1 == NULL)  goto ret_error_alloc;

#if !defined NDEBUG
         printf ("stack1 size: %d to %d\n", lenst1 >> 1, lenst1);
#endif
      }   // endif: stack is full
      else if (flag_inst[*iinst].dec_st1)  goto ret_error_stackrange;

   skip_check_stack1:

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

ret_error_stackrange:
ret_error_alloc:

   free (bsbst0);
   free (bsbst1);
   return 0;
}   // endfunc: void proc-bsbcode
